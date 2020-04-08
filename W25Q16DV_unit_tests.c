/*
 * W25Q16DV_unit_tests.c
 *
 *  Created on: Feb 8, 2020
 *      Author: Carlin R Connell
 */

#include "W25Q16DV_unit_tests.h"

Display_Handle debug;

char row[55];

void dump(uint8_t* addr, uint32_t len){
    Display_clear(debug);
    Display_doClear(debug);
    Display_printf(debug, 0, 0, "test3");
    int i;
    unsigned char *pc = (unsigned char*)addr;

     // <-- for some reason it doesnt like an array on the stack
    memset(row, 0x00, 55);

    char* pos = row;

    // process every byte of data
    for(i = 0; i < len; i++){

        // we need a new line
        if((i % 16) == 0){
            if (i != 0){
                // just dont print ASCII for the zeroth line.
                memset(row, 0x00, 55);
                Display_clear(debug);
                Display_doClear(debug);
                Display_printf(debug, 0, 0, "  %s", row);
                pos = row;
                memset(row, 0x00, 55);
            }

            // output offset.
            sprintf(pos, "  %04x", i);
            pos += 6;
            // Display_printf(debug, 1, 0, "  %04x", i);
            Display_clear(debug);
            Display_doClear(debug);
            Display_printf(debug, 0, 0, "test1");
        }
        Display_clear(debug);
        Display_doClear(debug);
        Display_printf(debug, 0, 0, "test2");
        // now print the hex code for the specific character.
        sprintf(pos, " %02x", pc[i]);
        pos += 3;
        //Display_printf(debug, 1, 0, " %02x", pc[i]);

        // add store a printable ASCII character for later.
        /*if((pc[i] < 0x20) || (pc[i] > 0x7e)){
            buf[i % 16] = '.';
        } else {
            buf[(i % 16) + 1] = '\0';
        }*/
    }

    // pad the last line
    while((i % 16) != 0){
        sprintf(pos, "   ");
        pos += 3;
    }
    memset(row, 0x00, 55);
    Display_printf(debug, 1, 0, "  %s", row);
    free(row);
}



void run_unit_tests_W25Q16DV(Display_Handle display, bool verbose){
    debug = display;

    Display_printf(debug, 0, 0, "----------------------------------------------------------");
    Display_printf(debug, 0, 0, "                 Running Tests on W25Q16DV                ");
    Display_printf(debug, 0, 0, "----------------------------------------------------------");
    W25Q16DV_init(display);



    uint8_t manufacturerID = 0;
    W2Q16DV_readManufacturer(&manufacturerID); // EFh
    Display_printf(debug, 0, 0, "Manufacturer ID: %xh", (unsigned int)manufacturerID);

    uint8_t deviceID;
    W2Q16DV_readDeviceID(&deviceID);          // 14h
    Display_printf(debug, 0, 0, "Device ID: %xh", (unsigned int)deviceID);

    uint64_t uniqueSerialNumber = 0;
    W2Q16DV_readUniqueIDNumber(&uniqueSerialNumber);
    Display_printf(debug, 0, 0, "Unique ID: %xh", (unsigned int)uniqueSerialNumber);

    uint8_t memoryType = 0;
    W2Q16DV_readMemoryType(&memoryType);
    Display_printf(debug, 0, 0, "Memory Type: %d", (unsigned int)memoryType);

    uint8_t capacity = 0;
    W2Q16DV_readCapacity(&capacity);
    Display_printf(debug, 0, 0, "Capacity: %d", (unsigned int)capacity);



    uint32_t addr = 0x00;
    uint8_t readBuf0[64];
    uint8_t readBuf1[64];
    uint8_t writeBuf[64];
    size_t n = sizeof(readBuf0);

    // testing read / erase / write
    Display_printf(debug, 0, 0, "....testing erase(), fastRead() and pageProgram()....");

    memset(writeBuf, 127, n);
    // dump(writeBuf, 64);

    Display_printf(debug, 0, 0, "erasing flash");
    W25Q16DV_eraseSector(0x00, true);

    memset(readBuf0, 0x00, n);
    W25Q16DV_read(addr, readBuf0, n);
    Display_printf(debug, 0, 0, "read from memory (expect 0xff)");
    // dump(readBuf, n);


    W25Q16DV_pageProgram(addr, addr, writeBuf, n);
    Display_printf(debug, 0, 0, "writing to memory");
    // dump(writeBuf, 64);

    memset(readBuf1, 0x00, n);
    Display_printf(debug, 0, 0, "read from memory (expect 0x7f)");
    W25Q16DV_read(addr, readBuf1, n);
    // dump(readBuf, n);


    // testing fast read / erase / write
    Display_printf(debug, 0, 0, "....testing erase(), fastRead() and pageProgram()....");

    memset(writeBuf, 127, n);
    // dump(writeBuf, 64);

    Display_printf(debug, 0, 0, "erasing flash");
    W25Q16DV_eraseSector(0x00, true);

    memset(readBuf0, 0x00, n);
    W25Q16DV_fastRead(addr, readBuf0, n);
    Display_printf(debug, 0, 0, "read from memory (expect 0xff)");
    // dump(readBuf, n);

    memset(readBuf1, 0x00, n);
    W25Q16DV_pageProgram(addr, addr, writeBuf, n);
    Display_printf(debug, 0, 0, "writing to memory");
    // dump(writeBuf, 64);

    memset(readBuf1, 0x00, n);
    Display_printf(debug, 0, 0, "read from memory (expect 0x7f)");
    W25Q16DV_fastRead(addr, readBuf1, n);
    // dump(readBuf, n);

    //
    uint8_t statusRegister1 = 0;
    Display_printf(debug, 0, 0, "....erase all (expect isBusy)....");
    W25Q16DV_eraseAll(false);

    if (W25Q16DV_isBusy()){


        W25Q16DV_eraseProgramSuspend(true);
        Display_printf(debug, 0, 0, "isBusy");
        Display_printf(debug, 0, 0, "eraseProgramSuspend (expect Not Busy and Suspended)");

        if (!W25Q16DV_isBusy()){

            Display_printf(debug, 0, 0, "Not Busy");
            if(W25Q16DV_isSuspended()){
                Display_printf(debug, 0, 0, "isSuspended");
            }else{
                Display_printf(debug, 0, 0, "Not Suspended");
            }

            Display_printf(debug, 0, 0, "eraseProgramResume (expect eraseOperation in place)");
            W25Q16DV_readStatusRegister1(&statusRegister1);


            W25Q16DV_eraseProgramResume(false);



        } else {

            Display_printf(debug, 0, 0, "isBusy");

        }

    }

    W25Q16DV_destroy();

}
