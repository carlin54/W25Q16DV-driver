/*
 * W25Q16V.h
 *
 *  Created on: Jul 17, 2019
 *      Author: Carlin R Connell
 */

#ifndef W25Q16DV_DRIVER_W25Q16DV_H_
#define W25Q16DV_DRIVER_W25Q16DV_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* POSIX Header files */
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/* Example/Board Header files */
#include "Board.h"

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>
#include <ti/display/Display.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerMSP432.h>
#include "Board.h"

#include "../Debug_ALL.h"

// Figure 3a. Status Register-1

/*! Status register 1, busy bit, bit mask. */
#define SR1_BUSY_MASK                                   0b00000001
/*! Status register 1, well bit, bit mask. */
#define SR1_WELL_MASK                                   0b00000010 // WRITE ENABLE LATCH

/*! Status register 1, block protection bit 0 (non-volatile), bit mask.*/
#define SR1_BP0_MASK                                    0b00000100 // BLOCK PROTECT BIT (non-volatile)

/*! Status register 1, block protection bit 1 (non-volatile), bit mask.*/
#define SR1_BP1_MASK                                    0b00001000 // BLOCK PROTECT BIT (non-volatile)

/*! Status register 1, block protection bit 2 (non-volatile), bit mask.*/
#define SR1_BP2_MASK                                    0b00010000 // BLOCK PROTECT BIT (non-volatile)

/*! Status register 1, top/bottom protect bit (non-volatile) bit mask.*/
#define SR1_TB_MASK                                     0b00100000 // TOP/BOTTOM PROTEXT (non-volatile)

/*! Status register 1, sector protect bit (non-volatile), bit mask.*/
#define SR1_SEC_MASK                                    0b01000000 // SECTOR PROTEXT (non-volatile)

/*! Status register 1, status register 0 protection bit (non-volatile), bit mask. */
#define SR1_SRP0_MASK                                   0b10000000 // STATUS REGISTER PROTECT 0

// Figure 3b. Status Register-2
/*! Status register 2, status register 1 protection bit (non-volatile), bit mask. */
#define SR2_SRP1_MASK                                   0b00000001 // STATUS REGISTER PROTECT 1 (non-volatile)

/*! Status register 2, quad enabled bit mask (non-volatile). */
#define SR2_QE_MASK                                     0b00000010 // QUAD ENABLE (non-volatile)

/*! Status register 2, security register lock bit 1 (non-volatile), bit mask.*/
#define SR2_LB1_MASK                                    0b00001000 // SECURITY REGISTER LOCK BIT (non-volatile)

/*! Status register 2, security register lock bit 2 (non-volatile), bit mask (non-volatile).*/
#define SR2_LB2_MASK                                    0b00010000 // SECURITY REGISTER LOCK BIT (non-volatile)

/*! Status register 2, security register lock bit 3 (non-volatile), bit mask. */
#define SR2_LB3_MASK                                    0b00100000 // SECURITY REGISTER LOCK BIT (non-volatile)

/*! Status register 2, complement protect bit (non-volatile), bit mask (non-volatile). */
#define SR2_CMP_MASK                                    0b01000000 // COMPLEMENT PROTECT (non-volatile)

/*! Status register 2, suspended status bit mask. */
#define SR2_SUS_MASK                                    0b10000000 // SUSPEND STATUS

/*! W25Q16DV sector size, the size of the sectors on the device. */
#define W25Q16DV_SECTOR_SIZE    4096 // 4096 bytes

/*! W25Q16DV page size, the max programmable in one instruction. */
#define W25Q16DV_PAGE_SIZE      256

void W25Q16DV_dump(uint8_t *dt, uint32_t n);

void W25Q16DV_select();

void W25Q16DV_deselect();

void W25Q16DV_init();

void W25Q16DV_destroy();

void W25Q16DV_set_transfer(size_t count, void* sendBuf, void* readBuf);

bool W25Q16DV_transfer();

bool W25Q16DV_send(void* sendBuf, size_t count);

bool W25Q16DV_recieve(void* readBuf, size_t count);

void W25Q16DV_writeEnable();

void W25Q16DV_writeEnableForVolatileStatusRegister();

void W25Q16DV_writeDisable();

void W25Q16DV_readStatusRegister1(uint8_t* statusRegister1);

void W25Q16DV_readStatusRegister2(uint8_t* statusRegister2);

void W25Q16DV_writeStatusRegister(uint8_t* writeStatusRegister);

void W25Q16DV_read(uint32_t addr, uint8_t* readBuf, size_t n);

void W25Q16DV_fastRead(uint32_t addr, uint8_t* readBuf, size_t n);

// -- Fast Read Dual Output (3Bh)

// -- Fast Read Quad Output (6bh)

// -- Fast Read Dual I/O (BBh)

// -- Fast Read Quad I/O (EBh)

// -- Word Read Quad I/O (E7h)

// -- Octal Read Quad I/O (E3h)

// -- Set Burst with Wrap (77h)

// -- Continuous Read Mode Bits (M7-0)

// -- Continuous Read Mode Reset (FFh or FFFFh)

void W25Q16DV_pageProgram(uint16_t sect_no, uint16_t inaddr, uint8_t* pageData, size_t n);

// -- Quad Input Page Program (32h)

void W25Q16DV_eraseSector(uint16_t sect_no, bool flgwait);

void W25Q16DV_32KB_blockErase(uint16_t blk_no, bool flgwait);

void W25Q16DV_64KB_blockErase(uint16_t blk_no, bool flgwait);

void W25Q16DV_eraseAll(bool flgwait);

void W25Q16DV_eraseProgramSuspend(bool flgwait);

void W25Q16DV_eraseProgramResume(bool flgwait);

void W25Q16DV_powerDown();

void W25Q16DV_releasePowerDown();

void W2Q16DV_readManufacturer(uint8_t* manufacturerID);

void W2Q16DV_readDeviceID(uint8_t* deviceID);

// -- Read Manufacturer / Device ID Dual I/O (92h)

// -- Read Manufacturer / Device ID Quad I/O (94h)

void W2Q16DV_readUniqueIDNumber(uint64_t* uniqueSerialNumber);

void W2Q16DV_readMemoryType(uint8_t* memoryType);

void W2Q16DV_readCapacity(uint8_t* capacity);

void W2Q16DV_readSFDPRegister(uint32_t addr, uint8_t* SFDPRegister);

void W2Q16DV_eraseSecurityRegisters(uint32_t addr);

void W2Q16DV_programSecurityRegisters(uint32_t addr, uint8_t* writeData, uint16_t n);

void W2Q16DV_readSecurityRegisters(uint32_t addr, uint8_t* readBuf, uint16_t n);

void W2Q16DV_enableReset();

void W2Q16DV_reset();

bool W25Q16DV_isBusy();

bool W25Q16DV_isWriteEnabled();

bool W25Q16DV_blockProtectionBit0();

bool W25Q16DV_blockProtectionBit1();

bool W25Q16DV_blockProtectionBit2();

bool W25Q16DV_topBottomProtect();

bool W25Q16DV_sectorProtect();

bool W25Q16DV_statusRegisterProtect0();

bool W25Q16DV_statusRegisterProtect1();

bool W25Q16DV_isQuadEnabled();

bool W25Q16DV_securityLockBit1();

bool W25Q16DV_securityLockBit2();

bool W25Q16DV_securityLockBit3();

bool W25Q16DV_complementProtect();

bool W25Q16DV_isSuspended();

#endif /* W25Q16DV_DRIVER_W25Q16DV_H_ */
