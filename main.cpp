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

	std::memcpy((void *)(intptr_t)dest, (void *)(intptr_t)source, interval);

	printf("source : %p\n", (void *)(intptr_t )source);
	printf("dest : %p\n", (void *)(intptr_t )dest);
	printf("interval : %x\n", interval);

	emu->EIP = 0x1b;
	emu->ESP = dest;
	

	gui->OpenWindow();
	
//	emu->ESP = 0xffff;
//	emu->DumpRegisters(32);
	//emulation
	while(true){
		int bit		= 32;
		uint8_t code	= emu->GetCode8(0);
		instruction_func_t* func;

#ifndef QUIET
		cout<<"emu: ";
		cout<<"EIP = "<<hex<<showbase<<emu->EIP<<", ";
		cout<<"Code = "<<(uint32_t)code<<endl;
#endif

		if(bit == 16){
			func = instructions16[code];
		}else if(bit == 32){
			func = instructions32[code];
		}else if(bit == 64){
			cout<<"64bitには対応していません。"<<endl;
			break;
		}
		
		if(func == NULL){
//			cout<<"命令("<<showbase<<(int)code<<")は実装されていません。"<<endl;
			// おふざけ
			cout<<showbase<<(int)code<<"...？知らない命令ですね。"<<endl;
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
		
//		emu->EIP.reg32++;
	}
	
	emu->DumpRegisters(32);
	emu->DumpMemory("memdump.bin");
	
	delete emu;
	cout<<"emulator deleted."<<endl;
	
	delete gui;
	delete disp;
	return 0;
}


