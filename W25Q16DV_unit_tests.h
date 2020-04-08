/*
 * W25Q16DV_unit_test.h
 *
 *  Created on: Jul 23, 2019
 *      Author: Administrator
 */

#ifndef W25Q16DV_DRIVER_W25Q16DV_UNIT_TESTS_H_
#define W25Q16DV_DRIVER_W25Q16DV_UNIT_TESTS_H_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


#include "W25Q16DV.h"

void dump(uint8_t* addr, uint32_t len);

void run_unit_tests_W25Q16DV(Display_Handle display, bool verbose);

#endif /* W25Q16DV_DRIVER_W25Q16DV_UNIT_TESTS_H_ */
