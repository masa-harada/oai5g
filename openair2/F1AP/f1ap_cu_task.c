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

/*! \file openair2/F1AP/CU_F1AP.c
* \brief data structures for F1 interface modules
* \author EURECOM/NTUST
* \date 2018
* \version 0.1
* \company Eurecom
* \email: navid.nikaein@eurecom.fr, raymond.knopp@eurecom.fr, bing-kai.hong@eurecom.fr
* \note
* \warning
*/

#include "f1ap_common.h"
#include "f1ap_handlers.h"
#include "f1ap_cu_interface_management.h"
#include "f1ap_cu_task.h"

extern RAN_CONTEXT_t RC;

f1ap_setup_req_t *f1ap_du_data_from_du;

void cu_task_handle_sctp_association_ind(instance_t instance, sctp_new_association_ind_t *sctp_new_association_ind) {
  // Nothing
}

void cu_task_handle_sctp_association_resp(instance_t instance, sctp_new_association_resp_t *sctp_new_association_resp) {

  DevAssert(sctp_new_association_resp != NULL);

  if (sctp_new_association_resp->sctp_state != SCTP_STATE_ESTABLISHED) {
    LOG_W(F1AP, "Received unsuccessful result for SCTP association (%u), instance %d, cnx_id %u\n",
              sctp_new_association_resp->sctp_state,
              instance,
              sctp_new_association_resp->ulp_cnx_id);

      //f1ap_handle_setup_message(instance, sctp_new_association_resp->sctp_state == SCTP_STATE_SHUTDOWN);
    return; // exit -1 for debugging 
  }

  // go to an init func
  f1ap_du_data_from_du = (f1ap_setup_req_t *)calloc(1, sizeof(f1ap_setup_req_t));
  // save the assoc id 
  f1ap_du_data_from_du->assoc_id         = sctp_new_association_resp->assoc_id;
  f1ap_du_data_from_du->sctp_in_streams  = sctp_new_association_resp->in_streams;
  f1ap_du_data_from_du->sctp_out_streams = sctp_new_association_resp->out_streams;
}

void cu_task_handle_sctp_data_ind(sctp_data_ind_t *sctp_data_ind) {
  int result;

  DevAssert(sctp_data_ind != NULL);

  f1ap_handle_message(sctp_data_ind->assoc_id, sctp_data_ind->stream,
                          sctp_data_ind->buffer, sctp_data_ind->buffer_length);

  result = itti_free(TASK_UNKNOWN, sctp_data_ind->buffer);
  AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
}

void cu_task_send_sctp_init_req(instance_t enb_id) {
  // 1. get the itti msg, and retrive the enb_id from the message
  // 2. use RC.rrc[enb_id] to fill the sctp_init_t with the ip, port
  // 3. creat an itti message to init

  LOG_I(CU_F1AP, "F1AP_CU_SCTP_REQ(create socket)\n");
  MessageDef  *message_p = NULL;

  message_p = itti_alloc_new_message (TASK_CU_F1, SCTP_INIT_MSG);
  message_p->ittiMsg.sctp_init.port = F1AP_PORT_NUMBER;
  message_p->ittiMsg.sctp_init.ppid = F1AP_SCTP_PPID;
  message_p->ittiMsg.sctp_init.ipv4 = 1;
  message_p->ittiMsg.sctp_init.ipv6 = 0;
  message_p->ittiMsg.sctp_init.nb_ipv4_addr = 1;
  message_p->ittiMsg.sctp_init.ipv4_address[0] = inet_addr(RC.rrc[enb_id]->eth_params_s.my_addr);
  /*
   * SR WARNING: ipv6 multi-homing fails sometimes for localhost.
   * * * * Disable it for now.
   */
  message_p->ittiMsg.sctp_init.nb_ipv6_addr = 0;
  message_p->ittiMsg.sctp_init.ipv6_address[0] = "0:0:0:0:0:0:0:1";

  itti_send_msg_to_task(TASK_SCTP, enb_id, message_p);
}


void *F1AP_CU_task(void *arg) {

  MessageDef *received_msg = NULL;
  int         result;

  LOG_I(CU_F1AP,"Starting F1AP at CU\n");

  itti_mark_task_ready(TASK_CU_F1);

  cu_task_send_sctp_init_req(0);

  while (1) {
    itti_receive_msg(TASK_CU_F1, &received_msg);
    switch (ITTI_MSG_ID(received_msg)) {

      case SCTP_NEW_ASSOCIATION_IND:
        LOG_I(CU_F1AP, "SCTP_NEW_ASSOCIATION_IND\n");
        cu_task_handle_sctp_association_ind(ITTI_MESSAGE_GET_INSTANCE(received_msg),
                                         &received_msg->ittiMsg.sctp_new_association_ind);
        break;

      case SCTP_NEW_ASSOCIATION_RESP:
        LOG_I(CU_F1AP, "SCTP_NEW_ASSOCIATION_RESP\n");
        cu_task_handle_sctp_association_resp(ITTI_MESSAGE_GET_INSTANCE(received_msg),
                                         &received_msg->ittiMsg.sctp_new_association_resp);
        break;

      case SCTP_DATA_IND:
        LOG_I(CU_F1AP, "SCTP_DATA_IND\n");
        cu_task_handle_sctp_data_ind(&received_msg->ittiMsg.sctp_data_ind);
        break;

      case F1AP_SETUP_RESP: // from rrc
        LOG_W(CU_F1AP, "F1AP_SETUP_RESP\n");
        // CU_send_f1setup_resp(ITTI_MESSAGE_GET_INSTANCE(received_msg),
        //                                       &F1AP_SETUP_RESP(received_msg));
        CU_send_F1_SETUP_RESPONSE(ITTI_MESSAGE_GET_INSTANCE(received_msg),
                                               &F1AP_SETUP_RESP(received_msg));
        break;


//    case F1AP_SETUP_RESPONSE: // This is from RRC
//    CU_send_F1_SETUP_RESPONSE(instance, *f1ap_setup_ind, &(F1AP_SETUP_RESP) f1ap_setup_resp)   
//        break;
        
//    case F1AP_SETUP_FAILURE: // This is from RRC
//    CU_send_F1_SETUP_FAILURE(instance, *f1ap_setup_ind, &(F1AP_SETUP_FAILURE) f1ap_setup_failure)   
//       break;

      case TERMINATE_MESSAGE:
        LOG_W(CU_F1AP, " *** Exiting CU_F1AP thread\n");
        itti_exit_task();
        break;

      default:
        LOG_E(CU_F1AP, "CU Received unhandled message: %d:%s\n",
                  ITTI_MSG_ID(received_msg), ITTI_MSG_NAME(received_msg));
        break;
    } // switch
    result = itti_free (ITTI_MSG_ORIGIN_ID(received_msg), received_msg);
    AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);

    received_msg = NULL;
  } // while

  return NULL;
}