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

#include "gtest/gtest.h"
extern "C" {
#include "openair2/LAYER2/NR_MAC_UE/mac_proto.h"
#include "executables/softmodem-common.h"
uint64_t get_softmodem_optmask(void)
{
  return 0;
}
static softmodem_params_t softmodem_params;
softmodem_params_t* get_softmodem_params(void)
{
  return &softmodem_params;
}
}
#include <cstdio>
#include "common/utils/LOG/log.h"

TEST(test_pcmax, test_mpr)
{
  // Inner PRB, MPR = 1.5, no delta MPR
  int prb_start = 4;
  int N_RB_UL = 51; // 10Mhz
  int nr_band = 20;
  float expected_power = 23 - (1.5 / 2);
  EXPECT_EQ(expected_power, nr_get_Pcmax(23, nr_band, FR1, 2, false, 1, N_RB_UL, false, 6, prb_start));

  // Outer PRB, MPR = 3, no delta MPR
  prb_start = 0;
  expected_power = 23 - (3.0 / 2);
  EXPECT_EQ(expected_power, nr_get_Pcmax(23, nr_band, FR1, 2, false, 1, N_RB_UL, false, 6, prb_start));

  // Outer PRB on band 28, MPR = 3, delta MPR = 0.5 dB
  N_RB_UL = 78;
  nr_band = 28;
  expected_power = 23 - ((3.0 + 0.5) / 2);
  EXPECT_EQ(expected_power, nr_get_Pcmax(23, nr_band, FR1, 2, false, 1, N_RB_UL, false, 100, prb_start));
}

TEST(test_pcmax, test_not_implemented)
{
  int N_RB_UL = 51;
  EXPECT_DEATH(nr_get_Pcmax(23, 20, FR1, 1, false, 1, N_RB_UL, false, 6, 0), "MPR for Pi/2 BPSK not implemented yet");
}

TEST(test_pcmax, test_pucch_max_power)
{
  // Format 2, transform precoding, MPR = 1
  int prb_start = 0;
  int N_RB_UL = 51; // 10Mhz
  float expected_power = 23 - (1.0 / 2);
  EXPECT_EQ(expected_power, nr_get_Pcmax(23, 20, FR1, 2, false, 1, N_RB_UL, true, 1, prb_start));

  // Other fromats, no transform precoding, MPR = 3
  expected_power = 23 - (3.0 / 2);
  EXPECT_EQ(expected_power, nr_get_Pcmax(23, 20, FR1, 2, false, 1, N_RB_UL, false, 1, prb_start));
}

TEST(test_pucch_power_state, test_accumulated_delta_pucch)
{
  NR_UE_MAC_INST_t mac = {0};
  NR_UE_UL_BWP_t current_UL_BWP = {0};
  current_UL_BWP.scs = 1;
  current_UL_BWP.BWPSize = 106;
  NR_PUCCH_ConfigCommon_t pucch_ConfigCommon = {0};
  mac.current_UL_BWP = &current_UL_BWP;
  mac.current_UL_BWP->pucch_ConfigCommon = &pucch_ConfigCommon;
  mac.nr_band = 20;
  NR_PUCCH_Config_t pucch_Config = {0};
  struct NR_PUCCH_PowerControl power_config;
  pucch_Config.pucch_PowerControl = &power_config;
  mac.G_b_f_c = 0;
  mac.pucch_power_control_initialized = true;

  int scs = 1;
  int sum_delta_pucch = 3;
  uint8_t format_type = 1;
  uint16_t nb_of_prbs = 1;
  uint8_t freq_hop_flag = 0;
  uint8_t add_dmrs_flag = 0;
  uint8_t N_symb_PUCCH = 12;
  int subframe_number = 0;
  int O_uci = 2;
  uint16_t start_prb = 0;
  int P_CMAX =
      nr_get_Pcmax(23, mac.nr_band, FR1, 2, false, current_UL_BWP.scs, current_UL_BWP.BWPSize, false, nb_of_prbs, start_prb);

  int pucch_power_prev = get_pucch_tx_power_ue(&mac,
                                               scs,
                                               &pucch_Config,
                                               sum_delta_pucch,
                                               format_type,
                                               nb_of_prbs,
                                               freq_hop_flag,
                                               add_dmrs_flag,
                                               N_symb_PUCCH,
                                               subframe_number,
                                               O_uci,
                                               start_prb);
  EXPECT_LT(pucch_power_prev, P_CMAX);
  for (int i = 0; i < 10; i++) {
    int pucch_power_state = mac.G_b_f_c;
    int pucch_power = get_pucch_tx_power_ue(&mac,
                                            scs,
                                            &pucch_Config,
                                            sum_delta_pucch,
                                            format_type,
                                            nb_of_prbs,
                                            freq_hop_flag,
                                            add_dmrs_flag,
                                            N_symb_PUCCH,
                                            subframe_number,
                                            O_uci,
                                            start_prb);
    if (pucch_power_prev == P_CMAX) {
      EXPECT_EQ(pucch_power_state, mac.G_b_f_c) << "PUCCH power control state increased after reaching max TX power";
    }
    EXPECT_LE(pucch_power, P_CMAX) << "PUUCH TX power above P_CMAX";
    EXPECT_EQ(std::min(P_CMAX, pucch_power_prev + sum_delta_pucch), pucch_power)
        << "PUCCH power expected to change by delta pucch only, between P_CMAX and P_CMIN";
    pucch_power_prev = pucch_power;

    if (i > 5) {
      EXPECT_EQ(pucch_power, P_CMAX) << "Expected to reach MAX PUCCH TX power";
    }
  }

  sum_delta_pucch = -15;
  for (int i = 0; i < 10; i++) {
    int pucch_power_state = mac.G_b_f_c;
    int pucch_power = get_pucch_tx_power_ue(&mac,
                                            scs,
                                            &pucch_Config,
                                            sum_delta_pucch,
                                            format_type,
                                            nb_of_prbs,
                                            freq_hop_flag,
                                            add_dmrs_flag,
                                            N_symb_PUCCH,
                                            subframe_number,
                                            O_uci,
                                            start_prb);
    EXPECT_LE(mac.G_b_f_c, pucch_power_state) << "PUCCH power control state increased with negative delta pucch";
    pucch_power_prev = pucch_power;
  }
}

int main(int argc, char** argv)
{
  logInit();
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
