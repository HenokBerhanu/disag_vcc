/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

/*! \file ra_procedures.c
 * \brief Routines for UE MAC-layer power control procedures
 * \author Francesco Mani
 * \date 2023
 * \version 0.1
 * \email: email@francescomani.it
 * \note
 * \warning
 */

#include "LAYER2/NR_MAC_UE/mac_proto.h"

#define DEFAULT_P0_NOMINAL_PUCCH_0_DBM 0
#define DEFAULT_DELTA_F_PUCCH_0_DB 0

// TODO: This should be part of mac instance
/* TS 38.213 9.2.5.2 UE procedure for multiplexing HARQ-ACK/SR and CSI in a PUCCH */
/* this is a counter of number of pucch format 4 per subframe */
static int nb_pucch_format_4_in_subframes[LTE_NUMBER_OF_SUBFRAMES_PER_FRAME] = {0};

/* TS 38.211 Table 6.4.1.3.3.2-1: DM-RS positions for PUCCH format 3 and 4 */
static const int nb_symbols_excluding_dmrs[11][2][2]
= {
/*                     No additional DMRS            Additional DMRS   */
/* PUCCH length      No hopping   hopping         No hopping   hopping */
/* index                  0          1                 0          1    */
/*    4     */    {{      3    ,     2   }   ,  {      3     ,    2    }},
/*    5     */    {{      3    ,     3   }   ,  {      3     ,    3    }},
/*    6     */    {{      4    ,     4   }   ,  {      4     ,    4    }},
/*    7     */    {{      5    ,     5   }   ,  {      5     ,    5    }},
/*    8     */    {{      6    ,     6   }   ,  {      6     ,    6    }},
/*    9     */    {{      7    ,     7   }   ,  {      7     ,    7    }},
/*   10     */    {{      8    ,     8   }   ,  {      6     ,    6    }},
/*   11     */    {{      9    ,     9   }   ,  {      7     ,    7    }},
/*   12     */    {{     10    ,    10   }   ,  {      8     ,    8    }},
/*   13     */    {{     11    ,    11   }   ,  {      9     ,    9    }},
/*   14     */    {{     12    ,    12   }   ,  {     10     ,   10    }},
};

static int get_deltatf(uint16_t nb_of_prbs,
                       uint8_t N_symb_PUCCH,
                       uint8_t freq_hop_flag,
                       uint8_t add_dmrs_flag,
                       int N_sc_ctrl_RB,
                       int O_UCI);

// Implementation of 6.2.4 Configured ransmitted power
// 3GPP TS 38.101-1 version 16.5.0 Release 16
// -
// The UE is allowed to set its configured maximum output power PCMAX,f,c for carrier f of serving cell c in each slot.
// The configured maximum output power PCMAX,f,c is set within the following bounds: PCMAX_L,f,c <=  PCMAX,f,c <=  PCMAX_H,f,c
// -
// Measurement units:
// - p_max:              dBm
// - delta_TC_c:         dB
// - P_powerclass:       dBm
// - delta_P_powerclass: dB
// - MPR_c:              dB
// - delta_MPR_c:        dB
// - delta_T_IB_c        dB
// - delta_rx_SRS        dB
// note:
// - Assuming:
// -- Powerclass 3 capable UE (which is default power class unless otherwise stated)
// -- Maximum power reduction (MPR_c) for power class 3
// -- no additional MPR (A_MPR_c)
int nr_get_Pcmax(NR_UE_MAC_INST_t *mac, int Qm, bool powerBoostPi2BPSK, int scs, int N_RB_UL, bool is_transform_precoding, int n_prbs, int start_prb)
{
  int nr_band = mac->nr_band;
  if(mac->frequency_range == FR1) {

    //TODO configure P-MAX from the upper layers according to 38.331
    int p_powerclass = 23; // dBm assuming poweclass 3 UE
    int p_emax = mac->p_Max != INT_MIN ? mac->p_Max : p_powerclass;
    int delta_P_powerclass = 0; // for powerclass 2 needs to be changed
    if(mac->p_Max && Qm == 1 && powerBoostPi2BPSK && (nr_band == 40 || nr_band == 41 || nr_band == 77 || nr_band == 78 || nr_band == 79)) {
      p_emax += 3;
      delta_P_powerclass -= 3;
    }

    // TODO to be set for CA and DC
    int delta_T_IB = 0;

    // TODO in case of band 41 and PRB allocation within 4MHz of the upper or lower limit of the band -> delta_TC = 1.5
    if(nr_band == 41)
      LOG_E(NR_MAC, "Need to implement delta_TC for band 41\n");
    int delta_TC = 0;

    float MPR = 0;
    frame_type_t frame_type = get_frame_type(nr_band, scs);
    if(compare_relative_ul_channel_bw(nr_band, scs, N_RB_UL, frame_type)) {
      int rb_low = (n_prbs / 2) > 1 ? (n_prbs / 2) : 1;
      int rb_high = N_RB_UL - rb_low - n_prbs;
      bool is_inner_rb = start_prb >= rb_low && start_prb <= rb_high && n_prbs <= ((N_RB_UL / 2) + (N_RB_UL & 1));
      // Table 6.2.2-1 in 38.101
      switch (Qm) {
        case 1 :
          AssertFatal(false, "MPR for Pi/2 BPSK not implemented yet\n");
          break;
        case 2 :
          if (is_transform_precoding) {
            if(!is_inner_rb)
              MPR = 1;
          }
          else {
            if(is_inner_rb)
              MPR = 1.5;
            else
              MPR = 3;
          }
          break;
        case 4 :
          if (is_transform_precoding) {
            if(is_inner_rb)
              MPR = 1;
            else
              MPR = 2;
          }
          else {
            if(is_inner_rb)
              MPR = 2;
            else
              MPR = 3;
          }
          break;
        case 6 :
          if (is_transform_precoding)
            MPR = 2.5;
          else
            MPR = 3.5;
          break;
        case 8 :
          if (is_transform_precoding)
            MPR = 4.5;
          else
            MPR = 6.5;
          break;
          break;
        default:
          AssertFatal(false, "Invalid Qm %d\n", Qm);
      }
    }

    int A_MPR = 0; // TODO too complicated to implement for now (see 6.2.3 in 38.101-1)
    int delta_rx_SRS = 0; // TODO for SRS
    int P_MPR = 0; // to ensure compliance with applicable electromagnetic energy absorption requirements

    float total_reduction = (MPR > A_MPR ? MPR : A_MPR) + delta_T_IB + delta_TC + delta_rx_SRS;
    if (P_MPR > total_reduction)
      total_reduction = P_MPR;
    int pcmax_high, pcmax_low;
    if(mac->p_Max) {
      pcmax_high = p_emax < (p_powerclass - delta_P_powerclass) ? p_emax : (p_powerclass - delta_P_powerclass);
      pcmax_low = (p_emax - delta_TC) < (p_powerclass - delta_P_powerclass - total_reduction) ?
                  (p_emax - delta_TC) : (p_powerclass - delta_P_powerclass - total_reduction);
    }
    else {
      pcmax_high = p_powerclass - delta_P_powerclass;
      pcmax_low = p_powerclass - delta_P_powerclass - total_reduction;
    }
    // TODO we need a strategy to select a value between minimum and maximum allowed PC_max
    int pcmax = (pcmax_low + pcmax_high) / 2;
    LOG_D(MAC, "Configured maximum output power:  %d dBm <= PCMAX %d dBm <= %d dBm \n", pcmax_low, pcmax, pcmax_high);
    return pcmax;
  }
  else {
    // FR2 TODO it is even more complex because it is radiated power
    return 23;
  }
}

// This is not entirely correct. In certain k2/k1/k0 settings we might postpone accumulating delta_PUCCH until next HARQ feedback
// slot. The correct way to do this would be to calculate the K_PUCCH (delta_PUCCH summation window end) for each PUCCH occasion and
// compare PUCCH transmission symbol with the reception symbol of the DCI containing delta_PUCCH to determine if the delta_PUCCH
// should be added at each occasion.
int get_sum_delta_pucch(NR_UE_MAC_INST_t *mac, int slot, frame_t frame)
{
  int delta_tpc_sum = 0;
  for (int i = 0; i < NR_MAX_HARQ_PROCESSES; i++) {
    if (mac->dl_harq_info[i].active && mac->dl_harq_info[i].ul_slot == slot && mac->dl_harq_info[i].ul_frame == frame) {
      delta_tpc_sum += mac->dl_harq_info[i].delta_pucch;
      mac->dl_harq_info[i].delta_pucch = 0;
    }
  }
  return delta_tpc_sum;
}

// PUCCH Power control according to 38.213 section 7.2.1
int16_t get_pucch_tx_power_ue(NR_UE_MAC_INST_t *mac,
                              int scs,
                              NR_PUCCH_Config_t *pucch_Config,
                              int sum_delta_pucch,
                              uint8_t format_type,
                              uint16_t nb_of_prbs,
                              uint8_t freq_hop_flag,
                              uint8_t add_dmrs_flag,
                              uint8_t N_symb_PUCCH,
                              int subframe_number,
                              int O_uci,
                              uint16_t start_prb)
{
  NR_UE_UL_BWP_t *current_UL_BWP = mac->current_UL_BWP;
  AssertFatal(current_UL_BWP && current_UL_BWP->pucch_ConfigCommon,
              "Missing configuration: need UL_BWP and pucch_ConfigCommon to calculate PUCCH tx power\n");
  int PUCCH_POWER_DEFAULT = 0;
  // p0_nominal is optional
  int16_t P_O_NOMINAL_PUCCH = DEFAULT_P0_NOMINAL_PUCCH_0_DBM;
  if (current_UL_BWP->pucch_ConfigCommon->p0_nominal != NULL) {
    P_O_NOMINAL_PUCCH = *current_UL_BWP->pucch_ConfigCommon->p0_nominal;
  }

  struct NR_PUCCH_PowerControl *power_config = pucch_Config ? pucch_Config->pucch_PowerControl : NULL;

  if (!power_config)
    return (PUCCH_POWER_DEFAULT);

  int16_t P_O_UE_PUCCH = 0;

  if (pucch_Config->spatialRelationInfoToAddModList != NULL) {  /* FFS TODO NR */
    LOG_D(MAC,"PUCCH Spatial relation infos are not yet implemented\n");
    return (PUCCH_POWER_DEFAULT);
  }

  int G_b_f_c = 0;
  if (power_config->p0_Set != NULL) {
    P_O_UE_PUCCH = power_config->p0_Set->list.array[0]->p0_PUCCH_Value; /* get from index 0 if no spatial relation set */
  }

  int P_O_PUCCH = P_O_NOMINAL_PUCCH + P_O_UE_PUCCH;

  int16_t delta_F_PUCCH = DEFAULT_DELTA_F_PUCCH_0_DB;
  long *delta_F_PUCCH_config = NULL;
  int DELTA_TF;
  uint16_t N_ref_PUCCH;
  int N_sc_ctrl_RB = 0;

  /* computing of pucch transmission power adjustment */
  switch (format_type) {
    case 0:
      N_ref_PUCCH = 2;
      DELTA_TF = 10 * log10(N_ref_PUCCH/N_symb_PUCCH);
      delta_F_PUCCH_config = power_config->deltaF_PUCCH_f0;
      break;
    case 1:
      N_ref_PUCCH = 14;
      DELTA_TF = 10 * log10(N_ref_PUCCH/N_symb_PUCCH * O_uci);
      delta_F_PUCCH_config = power_config->deltaF_PUCCH_f1;
      break;
    case 2:
      N_sc_ctrl_RB = 10;
      DELTA_TF = get_deltatf(nb_of_prbs, N_symb_PUCCH, freq_hop_flag, add_dmrs_flag, N_sc_ctrl_RB, O_uci);
      delta_F_PUCCH_config = power_config->deltaF_PUCCH_f2;
      break;
    case 3:
      N_sc_ctrl_RB = 14;
      DELTA_TF = get_deltatf(nb_of_prbs, N_symb_PUCCH, freq_hop_flag, add_dmrs_flag, N_sc_ctrl_RB, O_uci);
      delta_F_PUCCH_config = power_config->deltaF_PUCCH_f3;
      break;
    case 4:
      N_sc_ctrl_RB = 14/(nb_pucch_format_4_in_subframes[subframe_number]);
      DELTA_TF = get_deltatf(nb_of_prbs, N_symb_PUCCH, freq_hop_flag, add_dmrs_flag, N_sc_ctrl_RB, O_uci);
      delta_F_PUCCH_config = power_config->deltaF_PUCCH_f4;
      break;
    default:
    {
      LOG_E(MAC,"PUCCH unknown pucch format %d\n", format_type);
      return (0);
    }
  }
  if (delta_F_PUCCH_config != NULL) {
    delta_F_PUCCH = *delta_F_PUCCH_config;
  }

  // PUCCH shall be as specified for QPSK modulated DFT-s-OFDM of equivalent RB allocation (38.101-1)
  // TODO: P_CMAX for format 2
  int P_CMAX = nr_get_Pcmax(mac, 2, false, mac->current_UL_BWP->scs, mac->current_UL_BWP->BWPSize, true, 1, start_prb);
  int P_CMIN = -40; // TODO: minimum TX power, possibly 38.101-1 6.3.1
  int16_t pathloss = compute_nr_SSB_PL(mac, mac->ssb_measurements.ssb_rsrp_dBm);

  if (power_config->twoPUCCH_PC_AdjustmentStates && *power_config->twoPUCCH_PC_AdjustmentStates > 1) {
    LOG_E(MAC,"PUCCH power control adjustment states with 2 states not yet implemented\n");
    return (PUCCH_POWER_DEFAULT);
  }
  int M_pucch_component = (10 * log10((double)(pow(2,scs) * nb_of_prbs)));

  int16_t pucch_power_without_g_pucch = P_O_PUCCH + M_pucch_component + pathloss + delta_F_PUCCH + DELTA_TF;

  if (power_config->p0_Set == NULL) {
    if (mac->pucch_power_control_initialized == false) {
      // Initialize power control state
      // Assuming only sending on PCell
      NR_PRACH_RESOURCES_t* prach_resources = &mac->ra.prach_resources;
      float DELTA_P_rampup_requested = (prach_resources->RA_PREAMBLE_POWER_RAMPING_COUNTER - 1) * prach_resources->RA_PREAMBLE_POWER_RAMPING_STEP;
      float DELTA_P_rampup = P_CMAX - (P_O_PUCCH + pathloss + delta_F_PUCCH + DELTA_TF + sum_delta_pucch);
      DELTA_P_rampup = max(min(0, DELTA_P_rampup), DELTA_P_rampup_requested);
      mac->G_b_f_c = DELTA_P_rampup + sum_delta_pucch;
      mac->pucch_power_control_initialized = true;
    }
    else {
      // PUCCH closed loop power control state
      G_b_f_c = mac->G_b_f_c;
      if (!((pucch_power_without_g_pucch + G_b_f_c >= P_CMAX && sum_delta_pucch > 0) ||
        (pucch_power_without_g_pucch + G_b_f_c <= P_CMIN && sum_delta_pucch < 0))) {
        G_b_f_c += sum_delta_pucch;
      }
      mac->G_b_f_c = G_b_f_c;
    }
  }


  int pucch_power = min(P_CMAX, pucch_power_without_g_pucch + G_b_f_c);

  LOG_D(MAC, "PUCCH ( Tx power : %d dBm ) ( 10Log(...) : %d ) ( from Path Loss : %d ) ( delta_F_PUCCH : %d ) ( DELTA_TF : %d ) ( G_b_f_c : %d ) \n",
        pucch_power, M_pucch_component, pathloss, delta_F_PUCCH, DELTA_TF, G_b_f_c);

  return pucch_power;
}

static int get_deltatf(uint16_t nb_of_prbs,
                       uint8_t N_symb_PUCCH,
                       uint8_t freq_hop_flag,
                       uint8_t add_dmrs_flag,
                       int N_sc_ctrl_RB,
                       int O_UCI)
{
  int DELTA_TF;
  int O_CRC = compute_pucch_crc_size(O_UCI);
  int N_symb = N_symb_PUCCH < 4 ? N_symb_PUCCH : nb_symbols_excluding_dmrs[N_symb_PUCCH - 4][add_dmrs_flag][freq_hop_flag];
  float N_RE = nb_of_prbs * N_sc_ctrl_RB * N_symb;
  float K1 = 6;
  if (O_UCI + O_CRC < 12)
    DELTA_TF = 10 * log10((double)(((K1 * (O_UCI)) / N_RE)));
  else {
    float K2 = 2.4;
    float BPRE = (O_UCI + O_CRC) / N_RE;
    DELTA_TF = 10 * log10((double)(pow(2, (K2 * BPRE)) - 1));
  }
  return DELTA_TF;
}
