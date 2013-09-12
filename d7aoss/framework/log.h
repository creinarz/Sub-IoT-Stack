/*
 *  Created on: Nov 22, 2012
 *  Authors:
 * 		maarten.weyn@artesis.be
 *  	glenn.ergeerts@artesis.be
 */

#ifndef __LOG_H_
#define __LOG_H_


//#TODO define from cmake
//#define LOG_FWK_ENABLED
//#define LOG_DLL_ENABLED
//#define LOG_PHY_ENABLED
//#define LOG_NWL_ENABLED
//#define LOG_TRANS_ENABLED
//#define LOG_TRACE_ENABLED // TODO for CCS set compiler options --entry-param and --exit-param to "name" for this to work (manually for now)

#include "../phy/phy.h"
#include "../dll/dll.h"

#define LOG_TYPE_STRING 0x01
#define LOG_TYPE_DATA 0x02

#define LOG_TYPE_PHY_RX_RES 0x02
#define LOG_TYPE_PHY_RX_RES_SIZE 5

#define LOG_TYPE_DLL_RX_RES 0x03
#define LOG_TYPE_DLL_RX_RES_SIZE 2

// TODO: can be removed now log_print_string accepts a printf style format string?
//bool itoa(int32_t n, char* out);

void log_print_string(char* format, ...);
void log_print_clean(char* format, ...);
void log_print_data(uint8_t* message, uint8_t length);

void log_phy_rx_res(phy_rx_data_t* res);

void log_dll_rx_res(dll_rx_res_t* res);

#endif /* __LOG_H_ */
