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

void free_param_request(nfapi_nr_param_request_scf_t *msg)
{
  if (msg->vendor_extension) {
    free(msg->vendor_extension);
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
