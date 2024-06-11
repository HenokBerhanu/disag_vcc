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
/*! \file nfapi/open-nFAPI/fapi/src/nr_fapi_p5_utils.c
 * \brief
 * \author Ruben S. Silva
 * \date 2024
 * \version 0.1
 * \company OpenAirInterface Software Alliance
 * \email: contact@openairinterface.org, rsilva@allbesmart.pt
 * \note
 * \warning
 */
#include "nr_fapi_p5_utils.h"

void copy_vendor_extension_value(nfapi_vendor_extension_tlv_t *dst, const nfapi_vendor_extension_tlv_t *src)
{
  nfapi_tl_t *dst_tlv = (nfapi_tl_t *)dst;
  nfapi_tl_t *src_tlv = (nfapi_tl_t *)src;

  switch (dst_tlv->tag) {
    case VENDOR_EXT_TLV_2_TAG: {
      vendor_ext_tlv_2 *dst_ve = (vendor_ext_tlv_2 *)dst_tlv;
      vendor_ext_tlv_2 *src_ve = (vendor_ext_tlv_2 *)src_tlv;

      dst_ve->dummy = src_ve->dummy;
    } break;
    case VENDOR_EXT_TLV_1_TAG: {
      vendor_ext_tlv_1 *dst_ve = (vendor_ext_tlv_1 *)dst_tlv;
      vendor_ext_tlv_1 *src_ve = (vendor_ext_tlv_1 *)src_tlv;

      dst_ve->dummy = src_ve->dummy;
    } break;
  }
}

bool eq_param_request(const nfapi_nr_param_request_scf_t *unpacked_req, const nfapi_nr_param_request_scf_t *req)
{
  EQ(unpacked_req->header.message_id, req->header.message_id);
  EQ(unpacked_req->header.message_length, req->header.message_length);
  return true;
}

bool eq_param_response(const nfapi_nr_param_response_scf_t *unpacked_req, const nfapi_nr_param_response_scf_t *req)
{
  EQ(unpacked_req->header.message_id, req->header.message_id);
  EQ(unpacked_req->header.message_length, req->header.message_length);
  EQ(unpacked_req->num_tlv, req->num_tlv);
  EQ(unpacked_req->error_code, req->error_code);

  EQ_TLV(unpacked_req->cell_param.release_capability, req->cell_param.release_capability);

  EQ_TLV(unpacked_req->cell_param.phy_state, req->cell_param.phy_state);

  EQ_TLV(unpacked_req->cell_param.skip_blank_dl_config, req->cell_param.skip_blank_dl_config);

  EQ_TLV(unpacked_req->cell_param.skip_blank_ul_config, req->cell_param.skip_blank_ul_config);

  EQ_TLV(unpacked_req->cell_param.num_config_tlvs_to_report, req->cell_param.num_config_tlvs_to_report);

  for (int i = 0; i < unpacked_req->cell_param.num_config_tlvs_to_report.value; ++i) {
    EQ_TLV(unpacked_req->cell_param.config_tlvs_to_report_list[i], req->cell_param.config_tlvs_to_report_list[i]);
  }

  EQ_TLV(unpacked_req->carrier_param.cyclic_prefix, req->carrier_param.cyclic_prefix);

  EQ_TLV(unpacked_req->carrier_param.supported_subcarrier_spacings_dl, req->carrier_param.supported_subcarrier_spacings_dl);

  EQ_TLV(unpacked_req->carrier_param.supported_bandwidth_dl, req->carrier_param.supported_bandwidth_dl);

  EQ_TLV(unpacked_req->carrier_param.supported_subcarrier_spacings_ul, req->carrier_param.supported_subcarrier_spacings_ul);

  EQ_TLV(unpacked_req->carrier_param.supported_bandwidth_ul, req->carrier_param.supported_bandwidth_ul);

  EQ_TLV(unpacked_req->pdcch_param.cce_mapping_type, req->pdcch_param.cce_mapping_type);

  EQ_TLV(unpacked_req->pdcch_param.coreset_outside_first_3_of_ofdm_syms_of_slot,
         req->pdcch_param.coreset_outside_first_3_of_ofdm_syms_of_slot);

  EQ_TLV(unpacked_req->pdcch_param.coreset_precoder_granularity_coreset, req->pdcch_param.coreset_precoder_granularity_coreset);

  EQ_TLV(unpacked_req->pdcch_param.pdcch_mu_mimo, req->pdcch_param.pdcch_mu_mimo);

  EQ_TLV(unpacked_req->pdcch_param.pdcch_precoder_cycling, req->pdcch_param.pdcch_precoder_cycling);

  EQ_TLV(unpacked_req->pdcch_param.max_pdcch_per_slot, req->pdcch_param.max_pdcch_per_slot);

  EQ_TLV(unpacked_req->pucch_param.pucch_formats, req->pucch_param.pucch_formats);

  EQ_TLV(unpacked_req->pucch_param.max_pucchs_per_slot, req->pucch_param.max_pucchs_per_slot);

  EQ_TLV(unpacked_req->pdsch_param.pdsch_mapping_type, req->pdsch_param.pdsch_mapping_type);

  EQ_TLV(unpacked_req->pdsch_param.pdsch_allocation_types, req->pdsch_param.pdsch_allocation_types);

  EQ_TLV(unpacked_req->pdsch_param.pdsch_vrb_to_prb_mapping, req->pdsch_param.pdsch_vrb_to_prb_mapping);

  EQ_TLV(unpacked_req->pdsch_param.pdsch_cbg, req->pdsch_param.pdsch_cbg);

  EQ_TLV(unpacked_req->pdsch_param.pdsch_dmrs_config_types, req->pdsch_param.pdsch_dmrs_config_types);

  EQ_TLV(unpacked_req->pdsch_param.pdsch_dmrs_max_length, req->pdsch_param.pdsch_dmrs_max_length);

  EQ_TLV(unpacked_req->pdsch_param.pdsch_dmrs_additional_pos, req->pdsch_param.pdsch_dmrs_additional_pos);

  EQ_TLV(unpacked_req->pdsch_param.max_pdsch_tbs_per_slot, req->pdsch_param.max_pdsch_tbs_per_slot);

  EQ_TLV(unpacked_req->pdsch_param.max_number_mimo_layers_pdsch, req->pdsch_param.max_number_mimo_layers_pdsch);

  EQ_TLV(unpacked_req->pdsch_param.supported_max_modulation_order_dl, req->pdsch_param.supported_max_modulation_order_dl);

  EQ_TLV(unpacked_req->pdsch_param.max_mu_mimo_users_dl, req->pdsch_param.max_mu_mimo_users_dl);

  EQ_TLV(unpacked_req->pdsch_param.pdsch_data_in_dmrs_symbols, req->pdsch_param.pdsch_data_in_dmrs_symbols);

  EQ_TLV(unpacked_req->pdsch_param.premption_support, req->pdsch_param.premption_support);

  EQ_TLV(unpacked_req->pdsch_param.pdsch_non_slot_support, req->pdsch_param.pdsch_non_slot_support);

  EQ_TLV(unpacked_req->pusch_param.uci_mux_ulsch_in_pusch, req->pusch_param.uci_mux_ulsch_in_pusch);

  EQ_TLV(unpacked_req->pusch_param.uci_only_pusch, req->pusch_param.uci_only_pusch);

  EQ_TLV(unpacked_req->pusch_param.pusch_frequency_hopping, req->pusch_param.pusch_frequency_hopping);

  EQ_TLV(unpacked_req->pusch_param.pusch_dmrs_config_types, req->pusch_param.pusch_dmrs_config_types);

  EQ_TLV(unpacked_req->pusch_param.pusch_dmrs_max_len, req->pusch_param.pusch_dmrs_max_len);

  EQ_TLV(unpacked_req->pusch_param.pusch_dmrs_additional_pos, req->pusch_param.pusch_dmrs_additional_pos);

  EQ_TLV(unpacked_req->pusch_param.pusch_cbg, req->pusch_param.pusch_cbg);

  EQ_TLV(unpacked_req->pusch_param.pusch_mapping_type, req->pusch_param.pusch_mapping_type);

  EQ_TLV(unpacked_req->pusch_param.pusch_allocation_types, req->pusch_param.pusch_allocation_types);

  EQ_TLV(unpacked_req->pusch_param.pusch_vrb_to_prb_mapping, req->pusch_param.pusch_vrb_to_prb_mapping);

  EQ_TLV(unpacked_req->pusch_param.pusch_max_ptrs_ports, req->pusch_param.pusch_max_ptrs_ports);

  EQ_TLV(unpacked_req->pusch_param.max_pduschs_tbs_per_slot, req->pusch_param.max_pduschs_tbs_per_slot);

  EQ_TLV(unpacked_req->pusch_param.max_number_mimo_layers_non_cb_pusch, req->pusch_param.max_number_mimo_layers_non_cb_pusch);

  EQ_TLV(unpacked_req->pusch_param.supported_modulation_order_ul, req->pusch_param.supported_modulation_order_ul);

  EQ_TLV(unpacked_req->pusch_param.max_mu_mimo_users_ul, req->pusch_param.max_mu_mimo_users_ul);

  EQ_TLV(unpacked_req->pusch_param.dfts_ofdm_support, req->pusch_param.dfts_ofdm_support);

  EQ_TLV(unpacked_req->pusch_param.pusch_aggregation_factor, req->pusch_param.pusch_aggregation_factor);

  EQ_TLV(unpacked_req->prach_param.prach_long_formats, req->prach_param.prach_long_formats);

  EQ_TLV(unpacked_req->prach_param.prach_short_formats, req->prach_param.prach_short_formats);

  EQ_TLV(unpacked_req->prach_param.prach_restricted_sets, req->prach_param.prach_restricted_sets);

  EQ_TLV(unpacked_req->prach_param.max_prach_fd_occasions_in_a_slot, req->prach_param.max_prach_fd_occasions_in_a_slot);

  EQ_TLV(unpacked_req->measurement_param.rssi_measurement_support, req->measurement_param.rssi_measurement_support);

  return true;
}
void free_param_request(nfapi_nr_param_request_scf_t *msg)
{
  if (msg->vendor_extension) {
    free(msg->vendor_extension);
  }
}

void free_param_response(nfapi_nr_param_response_scf_t *msg)
{
  if (msg->vendor_extension) {
    free(msg->vendor_extension);
  }

  if (msg->cell_param.config_tlvs_to_report_list) {
    free(msg->cell_param.config_tlvs_to_report_list);
  }
}

void copy_param_request(const nfapi_nr_param_request_scf_t *src, nfapi_nr_param_request_scf_t *dst)
{
  dst->header.message_id = src->header.message_id;
  dst->header.message_length = src->header.message_length;
  if (src->vendor_extension) {
    dst->vendor_extension = calloc(1, sizeof(nfapi_vendor_extension_tlv_t));
    dst->vendor_extension->tag = src->vendor_extension->tag;
    dst->vendor_extension->length = src->vendor_extension->length;
    copy_vendor_extension_value(&dst->vendor_extension, &src->vendor_extension);
  }
}

void copy_param_response(const nfapi_nr_param_response_scf_t *src, nfapi_nr_param_response_scf_t *dst)
{
  dst->header.message_id = src->header.message_id;
  dst->header.message_length = src->header.message_length;
  if (src->vendor_extension) {
    dst->vendor_extension = calloc(1, sizeof(nfapi_vendor_extension_tlv_t));
    dst->vendor_extension->tag = src->vendor_extension->tag;
    dst->vendor_extension->length = src->vendor_extension->length;
    copy_vendor_extension_value(&dst->vendor_extension, &src->vendor_extension);
  }

  dst->error_code = src->error_code;
  dst->num_tlv = src->num_tlv;

  COPY_TLV(dst->cell_param.release_capability, src->cell_param.release_capability);

  COPY_TLV(dst->cell_param.phy_state, src->cell_param.phy_state);

  COPY_TLV(dst->cell_param.skip_blank_dl_config, src->cell_param.skip_blank_dl_config);

  COPY_TLV(dst->cell_param.skip_blank_ul_config, src->cell_param.skip_blank_ul_config);

  COPY_TLV(dst->cell_param.num_config_tlvs_to_report, src->cell_param.num_config_tlvs_to_report);

  if (src->cell_param.config_tlvs_to_report_list) {
    dst->cell_param.config_tlvs_to_report_list =
        calloc(src->cell_param.num_config_tlvs_to_report.value, sizeof(nfapi_uint8_tlv_t *));
    for (int i = 0; i < src->cell_param.num_config_tlvs_to_report.value; ++i) {
      COPY_TLV(dst->cell_param.config_tlvs_to_report_list[i], src->cell_param.config_tlvs_to_report_list[i]);
    }
  }

  COPY_TLV(dst->carrier_param.cyclic_prefix, src->carrier_param.cyclic_prefix);

  COPY_TLV(dst->carrier_param.supported_subcarrier_spacings_dl, src->carrier_param.supported_subcarrier_spacings_dl);

  COPY_TLV(dst->carrier_param.supported_bandwidth_dl, src->carrier_param.supported_bandwidth_dl);

  COPY_TLV(dst->carrier_param.supported_subcarrier_spacings_ul, src->carrier_param.supported_subcarrier_spacings_ul);

  COPY_TLV(dst->carrier_param.supported_bandwidth_ul, src->carrier_param.supported_bandwidth_ul);

  COPY_TLV(dst->pdcch_param.cce_mapping_type, src->pdcch_param.cce_mapping_type);

  COPY_TLV(dst->pdcch_param.coreset_outside_first_3_of_ofdm_syms_of_slot,
           src->pdcch_param.coreset_outside_first_3_of_ofdm_syms_of_slot);

  COPY_TLV(dst->pdcch_param.coreset_precoder_granularity_coreset, src->pdcch_param.coreset_precoder_granularity_coreset);

  COPY_TLV(dst->pdcch_param.pdcch_mu_mimo, src->pdcch_param.pdcch_mu_mimo);

  COPY_TLV(dst->pdcch_param.pdcch_precoder_cycling, src->pdcch_param.pdcch_precoder_cycling);

  COPY_TLV(dst->pdcch_param.max_pdcch_per_slot, src->pdcch_param.max_pdcch_per_slot);

  COPY_TLV(dst->pucch_param.pucch_formats, src->pucch_param.pucch_formats);

  COPY_TLV(dst->pucch_param.max_pucchs_per_slot, src->pucch_param.max_pucchs_per_slot);

  COPY_TLV(dst->pdsch_param.pdsch_mapping_type, src->pdsch_param.pdsch_mapping_type);

  COPY_TLV(dst->pdsch_param.pdsch_allocation_types, src->pdsch_param.pdsch_allocation_types);

  COPY_TLV(dst->pdsch_param.pdsch_vrb_to_prb_mapping, src->pdsch_param.pdsch_vrb_to_prb_mapping);

  COPY_TLV(dst->pdsch_param.pdsch_cbg, src->pdsch_param.pdsch_cbg);

  COPY_TLV(dst->pdsch_param.pdsch_dmrs_config_types, src->pdsch_param.pdsch_dmrs_config_types);

  COPY_TLV(dst->pdsch_param.pdsch_dmrs_max_length, src->pdsch_param.pdsch_dmrs_max_length);

  COPY_TLV(dst->pdsch_param.pdsch_dmrs_additional_pos, src->pdsch_param.pdsch_dmrs_additional_pos);

  COPY_TLV(dst->pdsch_param.max_pdsch_tbs_per_slot, src->pdsch_param.max_pdsch_tbs_per_slot);

  COPY_TLV(dst->pdsch_param.max_number_mimo_layers_pdsch, src->pdsch_param.max_number_mimo_layers_pdsch);

  COPY_TLV(dst->pdsch_param.supported_max_modulation_order_dl, src->pdsch_param.supported_max_modulation_order_dl);

  COPY_TLV(dst->pdsch_param.max_mu_mimo_users_dl, src->pdsch_param.max_mu_mimo_users_dl);

  COPY_TLV(dst->pdsch_param.pdsch_data_in_dmrs_symbols, src->pdsch_param.pdsch_data_in_dmrs_symbols);

  COPY_TLV(dst->pdsch_param.premption_support, src->pdsch_param.premption_support);

  COPY_TLV(dst->pdsch_param.pdsch_non_slot_support, src->pdsch_param.pdsch_non_slot_support);

  COPY_TLV(dst->pusch_param.uci_mux_ulsch_in_pusch, src->pusch_param.uci_mux_ulsch_in_pusch);

  COPY_TLV(dst->pusch_param.uci_only_pusch, src->pusch_param.uci_only_pusch);

  COPY_TLV(dst->pusch_param.pusch_frequency_hopping, src->pusch_param.pusch_frequency_hopping);

  COPY_TLV(dst->pusch_param.pusch_dmrs_config_types, src->pusch_param.pusch_dmrs_config_types);

  COPY_TLV(dst->pusch_param.pusch_dmrs_max_len, src->pusch_param.pusch_dmrs_max_len);

  COPY_TLV(dst->pusch_param.pusch_dmrs_additional_pos, src->pusch_param.pusch_dmrs_additional_pos);

  COPY_TLV(dst->pusch_param.pusch_cbg, src->pusch_param.pusch_cbg);

  COPY_TLV(dst->pusch_param.pusch_mapping_type, src->pusch_param.pusch_mapping_type);

  COPY_TLV(dst->pusch_param.pusch_allocation_types, src->pusch_param.pusch_allocation_types);

  COPY_TLV(dst->pusch_param.pusch_vrb_to_prb_mapping, src->pusch_param.pusch_vrb_to_prb_mapping);

  COPY_TLV(dst->pusch_param.pusch_max_ptrs_ports, src->pusch_param.pusch_max_ptrs_ports);

  COPY_TLV(dst->pusch_param.max_pduschs_tbs_per_slot, src->pusch_param.max_pduschs_tbs_per_slot);

  COPY_TLV(dst->pusch_param.max_number_mimo_layers_non_cb_pusch, src->pusch_param.max_number_mimo_layers_non_cb_pusch);

  COPY_TLV(dst->pusch_param.supported_modulation_order_ul, src->pusch_param.supported_modulation_order_ul);

  COPY_TLV(dst->pusch_param.max_mu_mimo_users_ul, src->pusch_param.max_mu_mimo_users_ul);

  COPY_TLV(dst->pusch_param.dfts_ofdm_support, src->pusch_param.dfts_ofdm_support);

  COPY_TLV(dst->pusch_param.pusch_aggregation_factor, src->pusch_param.pusch_aggregation_factor);

  COPY_TLV(dst->prach_param.prach_long_formats, src->prach_param.prach_long_formats);

  COPY_TLV(dst->prach_param.prach_short_formats, src->prach_param.prach_short_formats);

  COPY_TLV(dst->prach_param.prach_restricted_sets, src->prach_param.prach_restricted_sets);

  COPY_TLV(dst->prach_param.max_prach_fd_occasions_in_a_slot, src->prach_param.max_prach_fd_occasions_in_a_slot);

  COPY_TLV(dst->measurement_param.rssi_measurement_support, src->measurement_param.rssi_measurement_support);

  COPY_TL(dst->nfapi_config.p7_vnf_address_ipv4.tl, src->nfapi_config.p7_vnf_address_ipv4.tl);
  memcpy(dst->nfapi_config.p7_vnf_address_ipv4.address,
         src->nfapi_config.p7_vnf_address_ipv4.address,
         sizeof(dst->nfapi_config.p7_vnf_address_ipv4.address));

  COPY_TL(dst->nfapi_config.p7_vnf_address_ipv6.tl, src->nfapi_config.p7_vnf_address_ipv6.tl);
  memcpy(dst->nfapi_config.p7_vnf_address_ipv6.address,
         src->nfapi_config.p7_vnf_address_ipv6.address,
         sizeof(dst->nfapi_config.p7_vnf_address_ipv6.address));

  COPY_TLV(dst->nfapi_config.p7_vnf_port, src->nfapi_config.p7_vnf_port);

  COPY_TL(dst->nfapi_config.p7_pnf_address_ipv4.tl, src->nfapi_config.p7_pnf_address_ipv4.tl);
  memcpy(dst->nfapi_config.p7_pnf_address_ipv4.address,
         src->nfapi_config.p7_pnf_address_ipv4.address,
         sizeof(dst->nfapi_config.p7_pnf_address_ipv4.address));

  COPY_TL(dst->nfapi_config.p7_pnf_address_ipv6.tl, src->nfapi_config.p7_pnf_address_ipv6.tl);
  memcpy(dst->nfapi_config.p7_pnf_address_ipv6.address,
         src->nfapi_config.p7_pnf_address_ipv6.address,
         sizeof(dst->nfapi_config.p7_pnf_address_ipv6.address));

  COPY_TLV(dst->nfapi_config.p7_pnf_port, src->nfapi_config.p7_pnf_port);

  COPY_TLV(dst->nfapi_config.timing_window, src->nfapi_config.timing_window);

  COPY_TLV(dst->nfapi_config.timing_info_mode, src->nfapi_config.timing_info_mode);

  COPY_TLV(dst->nfapi_config.timing_info_period, src->nfapi_config.timing_info_period);

  COPY_TLV(dst->nfapi_config.dl_tti_timing_offset, src->nfapi_config.dl_tti_timing_offset);

  COPY_TLV(dst->nfapi_config.ul_tti_timing_offset, src->nfapi_config.ul_tti_timing_offset);

  COPY_TLV(dst->nfapi_config.ul_dci_timing_offset, src->nfapi_config.ul_dci_timing_offset);

  COPY_TLV(dst->nfapi_config.tx_data_timing_offset, src->nfapi_config.tx_data_timing_offset);
}
