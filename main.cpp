#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <cstdint>
#include <cstring>

#include "Emulator.h"
#include "GUI.h"
#include "device/Device.h"

#define DEBUG

//#define QUIET

#define INTERNAL_BOXFILL
//#define TEST_VRAM
#define HARIBOTE_UI

using namespace std;

Emulator	*emu;
GUI		*gui;
Display		*disp;

void printVram(unsigned char *vram){
	printf("vran[0] %x \n", vram[0]);
	printf("vran[1] %x \n", vram[1]);
	printf("vran[2] %x \n", vram[2]);
	printf("vran[3] %x \n", vram[3]);
	printf("vran[3] %x \n", vram[4]);
	printf("vran[3] %x \n", vram[5]);
	printf("vran[3] %x \n", vram[6]);
	printf("vran[3] %x \n", vram[7]);
	printf("vran[3] %x \n", vram[8]);
	printf("vran[3] %x \n", vram[9]);
	printf("vran[3] %x \n", vram[10]);
	printf("vran[3] %x \n", vram[11]);
	printf("vran[3] %x \n", vram[12]);
}
int main(int argc, char **argv){
	
	//TODO parse args
	
	emu = new Emulator();
	cout<<"emulator created."<<endl;
	
	disp = new Display(emu->memory + VRAM_ADDR);
	gui = new GUI(disp);


//	getchar();

	//iplとasmheadの処理
	emu->LoadBinary(argv[1], 0x100000, 1440 * 1024);
	std::memcpy(&emu->memory[0x280000], &emu->memory[0x104390], 512 * 1024);

	uint32_t source;
	uint32_t dest;
	uint32_t interval;
	std::memcpy(&source, &emu->memory[0x280014], 4);
	std::memcpy(&dest, &emu->memory[0x28000c], 4 ); source += 0x280000;
	std::memcpy(&interval, &emu->memory[0x280010], 4);

	std::memcpy(&emu->memory[dest], &emu->memory[source], interval);

/*	printf("source : %p\n", (void *)(intptr_t )source);
	printf("dest : %p\n", (void *)(intptr_t )dest);
	printf("interval : %x\n", interval);
*/

	emu->EIP = 0x1b;
	emu->ESP = dest;


	//set vmode scrnx scrny vram
	emu->memory[0xff2] = 8;
	emu->SetMemory16(0xff4, 320);
    emu->memory[0xff6] = 200;
	emu->SetMemory32(0xff8, 0xa0000);
//	emu->memory[0xff8] = 0xa0;

	gui->OpenWindow();
	
//	emu->ESP = 0xffff;
//	emu->DumpRegisters(32);
	//emulation
	for(size_t i = 0; true; i++){
	    emu->AL = emu->EAX;
		emu->instr.prefix = emu->parse_prefix(emu);

		emu->instr.opcode	= emu->memory[emu->EIP + emu->sgregs[1].base];
		instruction_func_t* func;

		//two byte opecode
		switch(emu->instr.opcode) {
			case 0x0f:
				emu->EIP++;
				emu->instr.opcode = (emu->instr.opcode << 8) + (uint8_t)emu->GetSignCode8(0);
		}

#ifndef QUIET
		cout<<"emu: ";
		cout<<"EIP = "<<hex<<showbase<<emu->EIP<<", ";
		cout<<"Code = "<<(uint32_t)emu->instr.opcode<<endl;
#endif

		if(emu->instr.prefix) {
			func = instructions32[emu->instr.opcode];
		}else {
			func = instructions16[emu->instr.opcode];
		}

		if(func == NULL){
			cout<<"命令("<<showbase<<(int)emu->instr.opcode<<")は実装されていません。"<<endl;
			break;
		}

		//execute
		func(emu);

		//if(i >= 500000){
		//	emu->EIP = 0;
		//	printVram(emu->memory + VRAM_ADDR);
		//}
		if(emu->EIP == 0){
			cout<<"EIP = 0になったので終了"<<endl;
			break;
		}
		
		if(emu->EIP > emu->GetMemSize()){
			cout<<"out of memory."<<endl;
			break;
		}
		printf("i %d \n", (int)i);

	}
	
	emu->DumpRegisters(32);
	emu->DumpMemory("memdump.bin");
	
	delete emu;
	cout<<"emulator deleted."<<endl;
	
	delete gui;
	delete disp;
	return 0;
}


