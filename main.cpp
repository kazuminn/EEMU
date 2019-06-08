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
	

	gui->OpenWindow();
	
//	emu->ESP = 0xffff;
//	emu->DumpRegisters(32);
	//emulation
	while(true){
		emu->instr.prefix = emu->parse_prefix(emu);

		emu->instr.opcode	= emu->memory[emu->EIP + emu->sgregs[1].base];
		instruction_func_t* func;

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
		
		if(emu->EIP == 0){
			cout<<"EIP = 0になったので終了"<<endl;
			break;
		}
		
		if(emu->EIP > emu->GetMemSize()){
			cout<<"out of memory."<<endl;
			break;
		}
		
	}
	
	emu->DumpRegisters(32);
	emu->DumpMemory("memdump.bin");
	
	delete emu;
	cout<<"emulator deleted."<<endl;
	
	delete gui;
	delete disp;
	return 0;
}


