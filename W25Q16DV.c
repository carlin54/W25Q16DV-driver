/*
 * W25Q16DV.cpp
 *
 *  Created on: Jul 17, 2019
 *      Author: Administrator
 */


#include "W25Q16DV.h"


// 7.2.2 - Instruction Set Table 1 (Erase, Program Instructions)
#define CMD_WRITE_ENABLE                                0x06
#define CMD_WRITE_ENABLE_FOR_VOLATILE_STATUS_REGISTER   0x50
#define CMD_WRITE_DISABLE                               0x04
#define CMD_READ_STATUS_REGISTER_1                      0x05
#define CMD_READ_STATUS_REGISTER_2                      0x35
#define CMD_WRITE_STATUS_REGISTER                       0x01
#define CMD_PAGE_PROGRAM                                0x02
#define CMD_QUAD_PAGE_PROGRAM                           0x32
#define CMD_SECTOR_ERASE_4KB                            0x20
#define CMD_32KB_BLOCK_ERASE                            0x52
#define CMD_64KB_BLOCK_ERASE                            0xD8
#define CMD_CHIP_ERASE_ALL                              0xC7 // or 0x60
#define CMD_ERASE_PROGRAM_SUSPEND                       0x75
#define CMD_ERASE_PROGRAM_RESUME                        0x7A
#define CMD_POWER_DOWN                                  0xB9
#define CMD_CONTINUOUS_READ_MODE_RESET                  0xFF

// 7.2.3 - Instruction Set Table 2 (Read Instructions)
#define CMD_READ_DATA                                   0x03
#define CMD_FAST_READ                                   0x0B
#define CMD_FAST_READ_DUAL_OUTPUT                       0x3B
#define CMD_FAST_READ_QUAD_OUTPUT                       0x6B
#define CMD_FAST_READ_DUAL_IO                           0xBB
#define CMD_FAST_READ_QUAD_IO                           0xEB
#define CMD_WORD_READ_DUAL_IO                           0xE7
#define CMD_OCTAL_WORD_READ_QUAD_IO                     0xE3
#define CMD_SET_BURST_WITH_WRAP                         0x77

// 7.2.4 - Instruction Set Table 3 (ID, Security Instructions)
#define CMD_RELEASE_POWER_DOWN_DEVICE_ID                0xAB
#define CMD_READ_MANFACTURER_DEVICE_ID                  0x90
#define CMD_READ_MANUFACTURER_DEVICE_ID_BY_DUAL_IO      0x92
#define CMD_READ_MANUFACTURER_DEVICE_ID_BY_QUAD_IO      0x94
#define CMD_READ_JEDEC_ID                               0x9F
#define CMD_READ_UNIQUE_ID_NUMBER                       0x4B
#define CMD_READ_SFDP_REGISTER                          0x5A
#define CMD_READ_SFDP_REG                               0x5A
#define CMD_ERASE_SECURITY_REGISTERS                    0x44
#define CMD_PROGRAM_SECURITY_REGISTERS                  0x42
#define CMD_READ_SECURITY_REGISTERS                     0x48
#define CMD_ENABLE_RESET                                0x66
#define CMD_RESET                                       0x99



#define DUMMY_BYTE  0

SPI_Handle W25Q16DV;
SPI_Transaction transaction;

void W25Q16DV_select(){

    GPIO_write(Board_SPI_MASTER_READY, 0);

}

void W25Q16DV_deselect(){

    GPIO_write(Board_SPI_MASTER_READY, 1);

}

void W25Q16DV_init(){

    GPIO_setConfig(Board_SPI_MASTER_READY, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW);

    GPIO_write(Board_SPI_MASTER_READY, 1);

    SPI_Params spiParams;
    SPI_Params_init(&spiParams);
    spiParams.frameFormat = SPI_POL0_PHA0;
    spiParams.bitRate = 10000000;
    W25Q16DV = SPI_open(Board_SPI_MASTER, &spiParams);
    if (W25Q16DV == NULL) {
        DEBUG_PRINT("Error initializing W25Q156DV as Master SPI\n");
        while (1);
    }
    else {
        DEBUG_PRINT("W25Q156DV Master SPI initialized\n");
    }

    GPIO_setConfig(Board_SPI_MASTER_READY, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW);

}

void W25Q16DV_destroy(){

    W25Q16DV_powerDown();
    SPI_close(W25Q16DV);

}


void W25Q16DV_set_transfer(size_t count, void* sendBuf, void* readBuf){

    transaction.count = count;
    transaction.txBuf = sendBuf;
    transaction.rxBuf = readBuf;

}

bool W25Q16DV_transfer(){

    return SPI_transfer(W25Q16DV, &transaction);

}

bool W25Q16DV_send(void* sendBuf, size_t count){

    W25Q16DV_set_transfer(count, sendBuf, NULL);
    return W25Q16DV_transfer();

}

bool W25Q16DV_recieve(void* readBuf, size_t count){

    W25Q16DV_set_transfer(count, NULL, readBuf);
    return W25Q16DV_transfer();

}

void W25Q16DV_writeEnable(){

    uint8_t sendBuf[1] = {CMD_WRITE_ENABLE};
    bool sentOK = false;

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    W25Q16DV_deselect();

    if (!sentOK) DEBUG_PRINT("W25Q16DV_writeEnable() - Failed to send instruction.");

}


void W25Q16DV_writeEnableForVolatileStatusRegister(){

    uint8_t sendBuf[1] = {CMD_WRITE_ENABLE_FOR_VOLATILE_STATUS_REGISTER};
    bool sentOK = false;

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    W25Q16DV_deselect();

    if (!sentOK) DEBUG_PRINT("W25Q16DV_writeEnableForVolatileStatusRegister() - Failed to send instruction.");

}

void W25Q16DV_writeDisable(){

    uint8_t sendBuf[1] = {CMD_WRITE_DISABLE};
    bool sentOK = false;

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    W25Q16DV_deselect();

    if (!sentOK) DEBUG_PRINT("W25Q16DV_writeDisable() - Failed to send instruction.");

}

void W25Q16DV_readStatusRegister1(uint8_t* statusRegister1){

    uint8_t sendBuf[1] = {CMD_READ_STATUS_REGISTER_1};
    uint8_t readBuf[2] = {0x00, 0x00};
    bool sentOK = false;
    bool readOK = false;

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    readOK = W25Q16DV_recieve((void*)readBuf, sizeof(readBuf));
    W25Q16DV_deselect();

    *statusRegister1 = readBuf[0];

    if (!sentOK) DEBUG_PRINT("W25Q16DV_readStatusRegister1() - Failed to send instruction.");
    if (!readOK) DEBUG_PRINT("W25Q16DV_readStatusRegister1() - Failed to read.");

}

void W25Q16DV_readStatusRegister2(uint8_t* statusRegister2){

    uint8_t sendBuf[1] = {CMD_READ_STATUS_REGISTER_2};
    uint8_t readBuf[2] = {0x00, 0x00};
    bool sentOK = false;
    bool readOK = false;

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    readOK = W25Q16DV_recieve((void*)readBuf, sizeof(readBuf));
    W25Q16DV_deselect();

    *statusRegister2 = readBuf[1];

    if (!sentOK) DEBUG_PRINT("W25Q16DV_readStatusRegister2() - Failed to send instruction.");
    if (!readOK) DEBUG_PRINT("W25Q16DV_readStatusRegister2() - Failed to read.");

}

void W25Q16DV_writeStatusRegister(uint8_t* writeStatusRegister){

    uint8_t sendBuf[3] = { CMD_WRITE_STATUS_REGISTER,
                           writeStatusRegister[0],
                           writeStatusRegister[1] };
    bool sentOK = false;

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    W25Q16DV_deselect();

    if (!sentOK) DEBUG_PRINT("W25Q16DV_writeStatusRegister() - Failed to write.");

}

void W25Q16DV_read(uint32_t addr, uint8_t* readBuf, size_t n){

    uint8_t sendBuf[4] = {CMD_READ_DATA,
                        ((addr >> 16)),
                        ((addr >> 8)   & 0xFF),
                        ((addr)        & 0xFF),
                        };
    bool sentOK = false;
    bool readOK = false;

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    readOK = W25Q16DV_recieve((void*)readBuf, n);
    W25Q16DV_deselect();

    if (!sentOK) DEBUG_PRINT("W25Q16DV_read() - Failed to send instruction.");
    if (!readOK) DEBUG_PRINT("W25Q16DV_read() - Failed to read data from memory.");

}

void W25Q16DV_fastRead(uint32_t addr, uint8_t* readBuf, size_t n){

    uint8_t sendBuf[5] = {CMD_FAST_READ,
                          ((addr >> 16)),
                          ((addr >> 8)   & 0xFF),
                          ((addr)        & 0xFF),
                          DUMMY_BYTE};
    bool sentOK = false;
    bool readOK = false;

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    readOK = W25Q16DV_recieve((void*)readBuf, n);
    W25Q16DV_deselect();

    if (!sentOK) DEBUG_PRINT("W25Q16DV_fastRead() - Failed to send instruction.");
    if (!readOK) DEBUG_PRINT("W25Q16DV_fastRead() - Failed to read data from memory.");

}

// -- Fast Read Dual Output (3Bh)

// -- Fast Read Quad Output (6bh)

// -- Fast Read Dual I/O (BBh)

// -- Fast Read Quad I/O (EBh)

// -- Word Read Quad I/O (E7h)

// -- Octal Read Quad I/O (E3h)

// -- Set Burst with Wrap (77h)

// -- Continuous Read Mode Bits (M7-0)

// -- Continuous Read Mode Reset (FFh or FFFFh)

void W25Q16DV_pageProgram(uint16_t sect_no, uint16_t inaddr, uint8_t* pageData, size_t n){

    if (W25Q16DV_isBusy()) return;

    uint32_t addr = sect_no;
    addr <<= 12;
    addr += inaddr;
    uint8_t sendBuf[4] = { CMD_PAGE_PROGRAM,
                          (addr >> 16) & 0xFF,
                          (addr >> 8 ) & 0xFF,
                          (addr      ) & 0xFF };
    bool sentOK = false;
    bool writeOK = false;

    W25Q16DV_writeEnable();

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    writeOK = W25Q16DV_send((void*)pageData, n);
    W25Q16DV_deselect();

    if (!sentOK) DEBUG_PRINT("W25Q16DV_pageProgram() - Failed to send instruction.");
    if (!writeOK) DEBUG_PRINT("W25Q16DV_pageProgram() - Failed to write page to memory.");

}

// -- Quad Input Page Program (32h)

void W25Q16DV_eraseSector(uint16_t sect_no, bool flgwait){

    uint32_t addr = sect_no;
    addr <<= 12;

    uint8_t sendBuf[4] = { CMD_SECTOR_ERASE_4KB,
                           (addr >> 16) & 0xFF,
                           (addr >> 8 ) & 0xFF,
                           (addr      ) & 0xFF };

    bool sentOK = false;

    W25Q16DV_writeEnable();

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    W25Q16DV_deselect();

    while (W25Q16DV_isBusy() & flgwait) {
        // sleep
    }

    if (!sentOK) DEBUG_PRINT("W25Q16DV_eraseSector() - Failed to send instruction.");

}

void W25Q16DV_32KB_blockErase(uint16_t blk_no, bool flgwait){

    uint32_t addr = blk_no;
    addr <<= 15;

    uint8_t sendBuf[4] = {CMD_32KB_BLOCK_ERASE,
                       (addr >> 16) & 0xFF,
                       (addr >> 8 ) & 0xFF,
                       (addr      ) & 0xFF};
    bool sentOK = false;

    W25Q16DV_writeEnable();

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    W25Q16DV_deselect();


    while (W25Q16DV_isBusy() & flgwait) {
        // sleep
    }

    if (!sentOK) DEBUG_PRINT("W25Q16DV_32KB_blockErase() - Failed to send instruction.");

}

void W25Q16DV_64KB_blockErase(uint16_t blk_no, bool flgwait){

    uint32_t addr = blk_no;
    addr <<= 16;
    uint8_t sendBuf[4] = {CMD_64KB_BLOCK_ERASE,
                       (addr >> 16) & 0xFF,
                       (addr >> 8 ) & 0xFF,
                       (addr      ) & 0xFF};
    bool sentOK = false;

    W25Q16DV_writeEnable();

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    W25Q16DV_deselect();

    while (W25Q16DV_isBusy() & flgwait) {
        // sleep
    }

    if (!sentOK) DEBUG_PRINT("W25Q16DV_64KB_blockErase() - Failed to send instruction.");

}

void W25Q16DV_eraseAll(bool flgwait){

    uint8_t sendBuf[1] = {CMD_CHIP_ERASE_ALL};

    bool sentOK = false;

    W25Q16DV_writeEnable();

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    W25Q16DV_deselect();


    while (W25Q16DV_isBusy() & flgwait) {
        // sleep
    }

    if (!sentOK) DEBUG_PRINT("W25Q16DV_eraseAll() - Failed to send instruction.");

}

void W25Q16DV_eraseProgramSuspend(bool flgwait){

    uint8_t sendBuf[1] = {CMD_ERASE_PROGRAM_SUSPEND};

    bool sentOK = false;

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    W25Q16DV_deselect();

    if (!sentOK) DEBUG_PRINT("W25Q16DV_eraseProgramSuspend() - Failed to send instruction.");

    while (W25Q16DV_isBusy() & flgwait) {
        // sleep
    }
}

void W25Q16DV_eraseProgramResume(bool flgwait){
    uint8_t sendBuf[1] = {CMD_ERASE_PROGRAM_RESUME};
    bool sentOK = false;

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    W25Q16DV_deselect();

    if (!sentOK) DEBUG_PRINT("W25Q16DV_eraseProgramResume() - Failed to send instruction.");

    while (W25Q16DV_isBusy() & flgwait) {
        // sleep
    }
}

void W25Q16DV_powerDown(){
    uint8_t sendBuf[1] = {CMD_RELEASE_POWER_DOWN_DEVICE_ID};
    bool sentOK = false;

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    W25Q16DV_deselect();

    if (!sentOK) DEBUG_PRINT("W25Q16DV_powerDown() - Failed to send instruction.");
}

void W2Q16DV_readManufacturer(uint8_t* manufacturerID){

    uint8_t sendBuf[4] = {CMD_READ_MANFACTURER_DEVICE_ID,
                        DUMMY_BYTE,
                        DUMMY_BYTE,
                        DUMMY_BYTE,
                        };
    uint8_t readBuf[2] = {0x00, 0x00};
    bool sentOK = false;
    bool readOK = false;

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    readOK = W25Q16DV_recieve((void*)readBuf, sizeof(readBuf));
    W25Q16DV_deselect();

    *manufacturerID = readBuf[0];

    if (!sentOK) DEBUG_PRINT("W2Q16DV_readManufacturer() - Failed to send instruction.");
    if (!readOK) DEBUG_PRINT("W2Q16DV_readManufacturer() - Failed to read.");

}

void W2Q16DV_readDeviceID(uint8_t* deviceID){

    uint8_t sendBuf[4] = { CMD_READ_MANFACTURER_DEVICE_ID,
                           DUMMY_BYTE,
                           DUMMY_BYTE,
                           DUMMY_BYTE };
    uint8_t readBuf[2] = {0x00, 0x00};
    bool sentOK = false;
    bool readOK = false;

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    readOK = W25Q16DV_recieve((void*)readBuf, sizeof(readBuf));
    W25Q16DV_deselect();

    *deviceID = readBuf[1];

    if (!sentOK) DEBUG_PRINT("W2Q16DV_readDeviceID() - Failed to send instruction.");
    if (!readOK) DEBUG_PRINT("W2Q16DV_readDeviceID() - Failed to read.");

}

// -- Read Manufacturer / Device ID Dual I/O (92h)

// -- Read Manufacturer / Device ID Quad I/O (94h)

void W2Q16DV_readUniqueIDNumber(uint64_t* uniqueSerialNumber){

    uint8_t sendBuf[5] = {CMD_READ_UNIQUE_ID_NUMBER,
                        DUMMY_BYTE,
                        DUMMY_BYTE,
                        DUMMY_BYTE,
                        DUMMY_BYTE};
    bool sentOK = false;
    bool readOK = false;

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    readOK = W25Q16DV_recieve((void*)uniqueSerialNumber, sizeof(uint64_t));
    W25Q16DV_deselect();

    if (!sentOK) DEBUG_PRINT("W2Q16DV_readUniqueIDNumber() - Failed to send instruction.");
    if (!readOK) DEBUG_PRINT("W2Q16DV_readUniqueIDNumber() - Failed to read.");

}

void W2Q16DV_readMemoryType(uint8_t* memoryType){

    uint8_t sendBuf[1] = {CMD_READ_JEDEC_ID};
    uint8_t readBuf[3] = {0x00, 0x00, 0x00};
    bool sentOK = false;
    bool readOK = false;

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    readOK = W25Q16DV_recieve((void*)readBuf, sizeof(readBuf));
    W25Q16DV_deselect();

    *memoryType = readBuf[1];

    if (!sentOK) DEBUG_PRINT("W2Q16DV_readMemoryType() - Failed to send instruction.");
    if (!readOK) DEBUG_PRINT("W2Q16DV_readMemoryType() - Failed to read.");

}

void W2Q16DV_readCapacity(uint8_t* capacity){

    uint8_t sendBuf[1] = {CMD_READ_JEDEC_ID};
    uint8_t readBuf[3] = {0x00, 0x00, 0x00};
    bool sentOK = false;
    bool readOK = false;

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    readOK = W25Q16DV_recieve((void*)readBuf, sizeof(readBuf));
    W25Q16DV_deselect();

    *capacity = readBuf[2];

    if (!sentOK) DEBUG_PRINT("W2Q16DV_readCapacity() - Failed to send instruction.");
    if (!readOK) DEBUG_PRINT("W2Q16DV_readCapacity() - Failed to read.");

}

void W2Q16DV_readSFDPRegister(uint32_t addr, uint8_t* SFDPRegister){

    uint8_t sendBuf[5] = { CMD_READ_SFDP_REGISTER,
                        (addr >> 16) & 0xFF,
                        (addr >> 8 ) & 0xFF,
                        (addr      ) & 0xFF,
                        DUMMY_BYTE };

    bool sentOK = false;
    bool readOK = false;

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    readOK = W25Q16DV_recieve((void*)SFDPRegister, sizeof(uint8_t));
    W25Q16DV_deselect();

    if (!sentOK) DEBUG_PRINT("W2Q16DV_readSFDPRegister() - Failed to send instruction.");
    if (!readOK) DEBUG_PRINT("W2Q16DV_readSFDPRegister() - Failed to read.");

}

void W2Q16DV_eraseSecurityRegisters(uint32_t addr){
    uint8_t sendBuf[4] = { CMD_ERASE_SECURITY_REGISTERS,
                        (addr >> 16) & 0xFF,
                        (addr >> 8 ) & 0xFF,
                        (addr      ) & 0xFF };
    bool sentOK = false;

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    W25Q16DV_deselect();

    if (!sentOK) DEBUG_PRINT("W2Q16DV_eraseSecurityRegisters() - Failed to send instruction.");

}

void W2Q16DV_programSecurityRegisters(uint32_t addr, uint8_t* writeData, uint16_t n){

    uint8_t sendBuf[4] = { CMD_PROGRAM_SECURITY_REGISTERS,
                         (addr >> 16) & 0xFF,
                         (addr >> 8 ) & 0xFF,
                         (addr      ) & 0xFF };

    bool sentOK = false;
    bool writeOK = false;

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    writeOK = W25Q16DV_send((void*)writeData, n);
    W25Q16DV_deselect();

    if (!sentOK) DEBUG_PRINT("W2Q16DV_programSecurityRegisters() - Failed to send instruction.");
    if (!writeOK) DEBUG_PRINT("W2Q16DV_programSecurityRegisters() - Failed to write data.");

}

void W2Q16DV_readSecurityRegisters(uint32_t addr, uint8_t* readBuf, uint16_t n){

    uint8_t sendBuf[5] = { CMD_READ_SECURITY_REGISTERS,
                         (addr >> 16) & 0xFF,
                         (addr >> 8 ) & 0xFF,
                         (addr      ) & 0xFF,
                         DUMMY_BYTE };

    bool sentOK = false;
    bool readOK = false;

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    readOK = W25Q16DV_recieve((void*)readBuf, n);
    W25Q16DV_deselect();

    if (!sentOK) DEBUG_PRINT("W2Q16DV_readSecurityRegisters() - Failed to send instruction.");
    if (!readOK) DEBUG_PRINT("W2Q16DV_readSecurityRegisters() - Failed to read.");

}

void W2Q16DV_enableReset(){

    uint8_t sendBuf[1] = { CMD_ENABLE_RESET };
    bool sentOK = false;

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    W25Q16DV_deselect();

    if (!sentOK) DEBUG_PRINT("W2Q16DV_enableReset() - Failed to send instruction.");

}

void W2Q16DV_reset(){

    uint8_t sendBuf[1] = { CMD_RESET };
    bool sentOK = false;

    W25Q16DV_select();
    sentOK = W25Q16DV_send((void*)sendBuf, sizeof(sendBuf));
    W25Q16DV_deselect();

    if (!sentOK) DEBUG_PRINT("W2Q16DV_reset() - Failed to send instruction.");

}

bool W25Q16DV_isBusy(){

    uint8_t sr1 = 0;
    W25Q16DV_readStatusRegister1(&sr1);
    return  SR1_BUSY_MASK & sr1;

}


bool W25Q16DV_isWriteEnabled(){

    uint8_t sr1 = 0;
    W25Q16DV_readStatusRegister1(&sr1);
    return  SR1_WELL_MASK & sr1;

}

bool W25Q16DV_blockProtectionBit0(){

    uint8_t sr1 = 0;
    W25Q16DV_readStatusRegister1(&sr1);
    return  SR1_BP0_MASK & sr1;

}

bool W25Q16DV_blockProtectionBit1(){

    uint8_t sr1 = 0;
    W25Q16DV_readStatusRegister1(&sr1);
    return  SR1_BP1_MASK & sr1;

}

bool W25Q16DV_blockProtectionBit2(){

    uint8_t sr1 = 0;
    W25Q16DV_readStatusRegister1(&sr1);
    return  SR1_BP2_MASK & sr1;

}
bool W25Q16DV_topBottomProtect(){

    uint8_t sr1 = 0;
    W25Q16DV_readStatusRegister1(&sr1);
    return  SR1_TB_MASK & sr1;

}

bool W25Q16DV_sectorProtect(){

    uint8_t sr1 = 0;
    W25Q16DV_readStatusRegister1(&sr1);
    return  SR1_SRP0_MASK & sr1;

}

bool W25Q16DV_statusRegisterProtect0(){

    uint8_t sr1 = 0;
    W25Q16DV_readStatusRegister1(&sr1);
    return  SR1_BP2_MASK & sr1;

}

bool W25Q16DV_statusRegisterProtect1(){

    uint8_t sr2 = 0;
    W25Q16DV_readStatusRegister2(&sr2);
    return  SR2_SRP1_MASK & sr2;

}
bool W25Q16DV_isQuadEnabled(){

    uint8_t sr2 = 0;
    W25Q16DV_readStatusRegister2(&sr2);
    return  SR2_QE_MASK & sr2;

}

bool W25Q16DV_securityLockBit1(){

    uint8_t sr2 = 0;
    W25Q16DV_readStatusRegister2(&sr2);
    return  SR2_LB1_MASK & sr2;

}

bool W25Q16DV_securityLockBit2(){

    uint8_t sr2 = 0;
    W25Q16DV_readStatusRegister2(&sr2);
    return  SR2_LB2_MASK & sr2;

}

bool W25Q16DV_securityLockBit3(){

    uint8_t sr2 = 0;
    W25Q16DV_readStatusRegister2(&sr2);
    return  SR2_LB3_MASK & sr2;

}

bool W25Q16DV_complementProtect(){

    uint8_t sr2 = 0;
    W25Q16DV_readStatusRegister2(&sr2);
    return  SR2_CMP_MASK & sr2;

}

bool W25Q16DV_isSuspended(){

    uint8_t sr2 = 0;
    W25Q16DV_readStatusRegister2(&sr2);
    return  SR2_SUS_MASK & sr2;

}


