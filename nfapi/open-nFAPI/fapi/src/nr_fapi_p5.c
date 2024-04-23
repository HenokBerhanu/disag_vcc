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
/*! \file nfapi/open-nFAPI/fapi/src/nr_fapi_p5.c
 * \brief
 * \author Ruben S. Silva
 * \date 2024
 * \version 0.1
 * \company OpenAirInterface Software Alliance
 * \email: contact@openairinterface.org, rsilva@allbesmart.pt
 * \note
 * \warning
 */
#include "nr_fapi.h"
#include "nr_fapi_p5.h"
#include "nr_fapi_p5_utils.h"
#include "debug.h"

bool isFAPIMessageIDValid(uint16_t id)
{
  // SCF 222.10.04 Table 3-5 PHY API message types
  return (id >= NFAPI_NR_PHY_MSG_TYPE_PARAM_REQUEST && id <= 0xFF) || id == NFAPI_NR_PHY_MSG_TYPE_START_RESPONSE
         || id == NFAPI_NR_PHY_MSG_TYPE_UL_NODE_SYNC || id == NFAPI_NR_PHY_MSG_TYPE_DL_NODE_SYNC
         || id == NFAPI_NR_PHY_MSG_TYPE_TIMING_INFO;
}

int fapi_nr_p5_message_header_unpack(uint8_t **pMessageBuf,
                                     uint32_t messageBufLen,
                                     void *pUnpackedBuf,
                                     uint32_t unpackedBufLen,
                                     nfapi_p4_p5_codec_config_t *config)
{
  uint8_t **pReadPackedMessage = pMessageBuf;
  nfapi_p4_p5_message_header_t *header = pUnpackedBuf;
  fapi_message_header_t fapi_msg;

  if(pMessageBuf == NULL || pUnpackedBuf == NULL || messageBufLen < NFAPI_HEADER_LENGTH || unpackedBufLen < sizeof(nfapi_p4_p5_message_header_t)){
    return -1;
  }
  uint8_t *end = *pMessageBuf + messageBufLen;
  // process the header
  int result =
      (pull8(pReadPackedMessage, &fapi_msg.num_msg, end) && pull8(pReadPackedMessage, &fapi_msg.opaque_handle, end)
       && pull16(pReadPackedMessage, &header->message_id, end) && pull32(pReadPackedMessage, &fapi_msg.message_length, end));
  DevAssert(fapi_msg.message_length <= 0xFFFF);
  header->message_length = fapi_msg.message_length;
  return (result);
}

uint8_t fapi_nr_p5_message_body_pack(nfapi_p4_p5_message_header_t *header,
                                     uint8_t **ppWritePackedMsg,
                                     uint8_t *end,
                                     nfapi_p4_p5_codec_config_t *config)
{
  uint8_t result = 0;

  // look for the specific message
  switch (header->message_id) {
    case NFAPI_NR_PHY_MSG_TYPE_PARAM_REQUEST:
      result = 0;
      break;

    case NFAPI_NR_PHY_MSG_TYPE_PARAM_RESPONSE:
      result = pack_nr_param_response(header, ppWritePackedMsg, end, config);
      break;

    case NFAPI_NR_PHY_MSG_TYPE_CONFIG_REQUEST:
      result = pack_nr_config_request(header, ppWritePackedMsg, end, config);
      break;

    case NFAPI_NR_PHY_MSG_TYPE_CONFIG_RESPONSE:
      result = pack_nr_config_response(header, ppWritePackedMsg, end, config);
      break;

    case NFAPI_NR_PHY_MSG_TYPE_START_REQUEST:
      result = pack_nr_start_request(header, ppWritePackedMsg, end, config);
      break;

    case NFAPI_NR_PHY_MSG_TYPE_START_RESPONSE:
      result = pack_nr_start_response(header, ppWritePackedMsg, end, config);
      break;

    case NFAPI_NR_PHY_MSG_TYPE_STOP_REQUEST:
      result = pack_nr_stop_request(header, ppWritePackedMsg, end, config);
      break;

    case NFAPI_NR_PHY_MSG_TYPE_STOP_INDICATION:
      result = pack_nr_stop_indication(header, ppWritePackedMsg, end, config);
      break;

    case NFAPI_NR_PHY_MSG_TYPE_ERROR_INDICATION:
      result = pack_nr_error_indication(header, ppWritePackedMsg, end, config);
      break;

    default: {
      AssertFatal(header->message_id >= 0x00 && header->message_id <= 0xFF,
                  "FAPI message IDs are defined between 0x00 and 0xFF the message provided 0x%02x, which is not a FAPI message",
                  header->message_id);
      break;
    }
  }
  return result;
}

int fapi_nr_p5_message_pack(void *pMessageBuf,
                            uint32_t messageBufLen,
                            void *pPackedBuf,
                            uint32_t packedBufLen,
                            nfapi_p4_p5_codec_config_t *config)
{
  nfapi_p4_p5_message_header_t *pMessageHeader = pMessageBuf;
  uint8_t *pWritePackedMessage = pPackedBuf;
  AssertFatal(isFAPIMessageIDValid(pMessageHeader->message_id),
              "FAPI message IDs are defined between 0x00 and 0xFF the message provided 0x%02x, which is not a FAPI message",
              pMessageHeader->message_id);
  uint32_t packedMsgLen;
  uint32_t packedBodyLen;
  uint16_t packedMsgLen16;
  AssertFatal(pMessageBuf != NULL && pPackedBuf != NULL, "P5 Pack supplied pointers are null");

  uint8_t *pPackMessageEnd = pPackedBuf + packedBufLen;
  uint8_t *pPackedLengthField = &pWritePackedMessage[4];
  uint8_t *pPacketBodyField = &pWritePackedMessage[8];
  uint8_t *pPacketBodyFieldStart = &pWritePackedMessage[8];

  uint8_t res = fapi_nr_p5_message_body_pack(pMessageHeader, &pPacketBodyField, pPackMessageEnd, config);
  AssertFatal(res >= 0, "fapi_nr_p5_message_body_pack error packing message body %d\n", res);

  // PHY API message header
  push8(1, &pWritePackedMessage, pPackMessageEnd); // Number of messages
  push8(0, &pWritePackedMessage, pPackMessageEnd); // Opaque handle

  // PHY API Message structure
  push16(pMessageHeader->message_id, &pWritePackedMessage, pPackMessageEnd); // Message type ID

  // check for a valid message length
  packedMsgLen = get_packed_msg_len((uintptr_t)pPackedBuf, (uintptr_t)pPacketBodyField);
  packedBodyLen = get_packed_msg_len((uintptr_t)pPacketBodyFieldStart, (uintptr_t)pPacketBodyField);
  packedMsgLen16 = (uint16_t)packedBodyLen;
  if (pMessageHeader->message_id == NFAPI_NR_PHY_MSG_TYPE_PARAM_REQUEST
      || pMessageHeader->message_id == NFAPI_NR_PHY_MSG_TYPE_START_REQUEST
      || pMessageHeader->message_id == NFAPI_NR_PHY_MSG_TYPE_STOP_REQUEST
      || pMessageHeader->message_id == NFAPI_NR_PHY_MSG_TYPE_STOP_INDICATION) {
    // These messages don't have a body, length is 0
    packedMsgLen16 = 0;
  }
  AssertFatal(packedMsgLen <= 0xFFFF && packedMsgLen <= packedBufLen,
              "Packed message 0x%02x length error %d, buffer supplied %d\n",
              pMessageHeader->message_id,
              packedMsgLen,
              packedBufLen);
  // Update the message length in the header
  if (!push32(packedMsgLen16, &pPackedLengthField, pPackMessageEnd))
    return -1;

  // return the packed length
  return (int)(packedMsgLen);
}

int fapi_nr_p5_message_unpack(void *pMessageBuf,
                              uint32_t messageBufLen,
                              void *pUnpackedBuf,
                              uint32_t unpackedBufLen,
                              nfapi_p4_p5_codec_config_t *config)
{
  fapi_message_header_t *pMessageHeader = pUnpackedBuf;
  uint8_t *pReadPackedMessage = pMessageBuf;

  AssertFatal(pMessageBuf != NULL && pUnpackedBuf != NULL, "P5 unpack supplied pointers are null");
  uint8_t *end = (uint8_t *)pMessageBuf + messageBufLen;
  AssertFatal(messageBufLen >= NFAPI_HEADER_LENGTH && unpackedBufLen >= sizeof(fapi_message_header_t),
              "P5 unpack supplied message buffer is too small %d, %d\n",
              messageBufLen,
              unpackedBufLen);
  // clean the supplied buffer for - tag value blanking
  (void)memset(pUnpackedBuf, 0, unpackedBufLen);
  if (fapi_nr_p5_message_header_unpack(&pReadPackedMessage, NFAPI_HEADER_LENGTH, pMessageHeader, sizeof(fapi_message_header_t), 0)
      < 0) {
    // failed to read the header
    return -1;
  }

  int result = -1;

  if (check_nr_fapi_unpack_length(pMessageHeader->message_id, unpackedBufLen) == 0) {
    // the unpack buffer is not big enough for the struct
    return -1;
  }

  // look for the specific message
  switch (pMessageHeader->message_id) {
    case NFAPI_NR_PHY_MSG_TYPE_PARAM_REQUEST:
      // PARAM request has no body;
      result = 0;
      break;

    case NFAPI_NR_PHY_MSG_TYPE_PARAM_RESPONSE:
      result = unpack_nr_param_response(&pReadPackedMessage, end, pMessageHeader, config);
      break;

    case NFAPI_NR_PHY_MSG_TYPE_CONFIG_REQUEST:
      result = unpack_nr_config_request(&pReadPackedMessage, end, pMessageHeader, config);
      break;

    case NFAPI_NR_PHY_MSG_TYPE_CONFIG_RESPONSE:
      result = unpack_nr_config_response(&pReadPackedMessage, end, pMessageHeader, config);
      break;

    case NFAPI_NR_PHY_MSG_TYPE_START_REQUEST:
      result = unpack_nr_start_request(&pReadPackedMessage, end, pMessageHeader, config);
      break;

    case NFAPI_NR_PHY_MSG_TYPE_START_RESPONSE:
      result = unpack_nr_start_response(&pReadPackedMessage, end, pMessageHeader, config);
      break;

    case NFAPI_NR_PHY_MSG_TYPE_STOP_REQUEST:
      result = unpack_nr_stop_request(&pReadPackedMessage, end, pMessageHeader, config);
      break;

    case NFAPI_NR_PHY_MSG_TYPE_STOP_INDICATION:
      result = unpack_nr_stop_indication(&pReadPackedMessage, end, pMessageHeader, config);
      break;

    case NFAPI_NR_PHY_MSG_TYPE_ERROR_INDICATION:
      result = unpack_nr_error_indication(&pReadPackedMessage, end, pMessageHeader, config);
      break;

    default:
      if (pMessageHeader->message_id >= NFAPI_VENDOR_EXT_MSG_MIN && pMessageHeader->message_id <= NFAPI_VENDOR_EXT_MSG_MAX) {
        NFAPI_TRACE(NFAPI_TRACE_ERROR,
                    "%s VE NFAPI message ID %d. No ve decoder provided\n",
                    __FUNCTION__,
                    pMessageHeader->message_id);
      } else {
        NFAPI_TRACE(NFAPI_TRACE_ERROR, "%s NFAPI Unknown P5 message ID %d\n", __FUNCTION__, pMessageHeader->message_id);
      }

      break;
  }

  return result;
}

int check_nr_fapi_unpack_length(nfapi_nr_phy_msg_type_e msgId, uint32_t unpackedBufLen)
{
  int retLen = 0;
  /**  NFAPI_NR_PHY_MSG_TYPE_PARAM_REQUEST=  0x00,
    NFAPI_NR_PHY_MSG_TYPE_PARAM_RESPONSE= 0x01,
    NFAPI_NR_PHY_MSG_TYPE_CONFIG_REQUEST= 0x02,
    NFAPI_NR_PHY_MSG_TYPE_CONFIG_RESPONSE=0X03,
    NFAPI_NR_PHY_MSG_TYPE_START_REQUEST=  0X04,
    NFAPI_NR_PHY_MSG_TYPE_STOP_REQUEST=   0X05,
    NFAPI_NR_PHY_MSG_TYPE_STOP_INDICATION=0X06,
    NFAPI_NR_PHY_MSG_TYPE_ERROR_INDICATION=0X07
    */
  // check for size of nFAPI struct without the nFAPI specific parameters
  switch (msgId) {
    case NFAPI_NR_PHY_MSG_TYPE_PARAM_REQUEST:
      if (unpackedBufLen >= sizeof(nfapi_nr_param_request_scf_t) - sizeof(nfapi_vendor_extension_tlv_t))
        retLen = sizeof(fapi_message_header_t);

      break;
    case NFAPI_NR_PHY_MSG_TYPE_PARAM_RESPONSE:
      if (unpackedBufLen >= sizeof(nfapi_nr_param_response_scf_t) - sizeof(nfapi_vendor_extension_tlv_t) - sizeof(nfapi_nr_nfapi_t))
        retLen = sizeof(nfapi_nr_param_request_scf_t);

      break;
    case NFAPI_NR_PHY_MSG_TYPE_CONFIG_REQUEST:
      if (unpackedBufLen >= sizeof(nfapi_nr_config_request_scf_t) - sizeof(nfapi_vendor_extension_tlv_t) - sizeof(nfapi_nr_nfapi_t))
        retLen = sizeof(nfapi_nr_config_request_scf_t);

      break;
    case NFAPI_NR_PHY_MSG_TYPE_CONFIG_RESPONSE:
      if (unpackedBufLen >= sizeof(nfapi_nr_config_response_scf_t) - sizeof(nfapi_vendor_extension_tlv_t))
        retLen = sizeof(nfapi_nr_config_response_scf_t);

      break;
    case NFAPI_NR_PHY_MSG_TYPE_START_REQUEST:
      if (unpackedBufLen >= sizeof(nfapi_nr_start_request_scf_t) - sizeof(nfapi_vendor_extension_tlv_t))
        retLen = sizeof(fapi_message_header_t);

      break;
    case NFAPI_NR_PHY_MSG_TYPE_START_RESPONSE:
      if (unpackedBufLen >= sizeof(nfapi_nr_start_response_scf_t) - sizeof(nfapi_vendor_extension_tlv_t))
        retLen = sizeof(fapi_message_header_t);

      break;
    case NFAPI_NR_PHY_MSG_TYPE_STOP_REQUEST:
      if (unpackedBufLen >= sizeof(nfapi_nr_stop_request_scf_t) - sizeof(nfapi_vendor_extension_tlv_t))
        retLen = sizeof(fapi_message_header_t);

      break;
    case NFAPI_NR_PHY_MSG_TYPE_STOP_INDICATION:
      if (unpackedBufLen >= sizeof(nfapi_nr_stop_indication_scf_t) - sizeof(nfapi_vendor_extension_tlv_t))
        retLen = sizeof(fapi_message_header_t);

      break;
    case NFAPI_NR_PHY_MSG_TYPE_ERROR_INDICATION:
      if (unpackedBufLen >= sizeof(nfapi_nr_error_indication_scf_t) - sizeof(nfapi_vendor_extension_tlv_t))
        retLen = sizeof(fapi_message_header_t);

      break;
    default:
      NFAPI_TRACE(NFAPI_TRACE_ERROR, "%s Unknown message ID %d\n", __FUNCTION__, msgId);
      break;
  }

  return retLen;
}
