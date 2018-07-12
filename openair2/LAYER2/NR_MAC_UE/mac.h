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

/*! \file LAYER2/MAC/defs.h
* \brief MAC data structures, constant, and function prototype
* \author Navid Nikaein and Raymond Knopp
* \date 2011
* \version 0.5
* \email navid.nikaein@eurecom.fr

*/
/** @defgroup _oai2  openair2 Reference Implementation
 * @ingroup _ref_implementation_
 * @{
 */

/*@}*/

#ifndef __LAYER2_NR_UE_MAC_DEFS_H__
#define __LAYER2_NR_UE_MAC_DEFS_H__



#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/** @defgroup _mac  MAC
 * @ingroup _oai2
 * @{
 */

/*!\brief Values of BCCH logical channel (fake)*/
#define NR_BCCH_DL_SCH 3			// SI

/*!\brief Values of PCCH logical channel (fake) */
#define NR_BCCH_BCH 5			// MIB
/*@}*/





/*!\brief UE layer 2 status */
typedef enum {
    CONNECTION_OK = 0,
    CONNECTION_LOST,
    PHY_RESYNCH,
    PHY_HO_PRACH
} UE_L2_STATE_t;








#endif /*__LAYER2_MAC_DEFS_H__ */