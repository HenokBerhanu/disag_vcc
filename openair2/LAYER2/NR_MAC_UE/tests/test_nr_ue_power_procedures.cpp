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
uint64_t get_softmodem_optmask(void) {return 0;}
static softmodem_params_t softmodem_params;
softmodem_params_t *get_softmodem_params(void) {
  return &softmodem_params;

}
}
#include <cstdio>
#include "common/utils/LOG/log.h"

TEST(power_procedures_fr1, test_prach_max_tx_power_mpr)
{
  // Inner PRB, MPR = 1.5
  int prb_start = 4;
  int N_RB_UL = 51; // 10Mhz
  EXPECT_EQ(22, nr_get_Pcmax(23, 20, FR1, 2, false, 1, N_RB_UL, false, 6, prb_start));

  // Outer PRB, MPR = 3
  prb_start = 0;
  EXPECT_EQ(21, nr_get_Pcmax(23, 20, FR1, 2, false, 1, N_RB_UL, false, 6, prb_start));

  // Channel bandwidth conditon not met, no MPR
  N_RB_UL = 106;
  EXPECT_EQ(23, nr_get_Pcmax(23, 20, FR1, 2, false, 1, N_RB_UL, false, 6, prb_start));
}

TEST(power_procedures_fr1, test_not_implemented)
{
  int N_RB_UL = 51;
  EXPECT_DEATH(nr_get_Pcmax(23, 20, FR1, 1, false, 1, N_RB_UL, false, 6, 0), "MPR for Pi/2 BPSK not implemented yet");
}

int main(int argc, char** argv)
{
  logInit();
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
