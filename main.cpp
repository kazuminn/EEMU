#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ucontext.h>
#include <signal.h>
#include <sys/wait.h>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <cstdio>

#include "Emulator.h"
#include "interrupt.h"
#include "device/PIC.h"
#include "GUI.h"
#include "device/Device.h"
#include "device/keyboard.h"
#include "queue"

#define DEBUG

#define QUIET

#define INTERNAL_BOXFILL
//#define TEST_VRAM
#define HARIBOTE_UI

using namespace std;


Emulator	*emu;
PIC	*pic;
keyboard *kb;
Interrupt	*inter;
GUI		*gui;
Display		*disp;


extern "C" void _pc(uintptr_t, int);

typedef struct _sig_ucontext {
 	unsigned long     uc_flags;
 	struct ucontext   *uc_link;
 	stack_t           uc_stack;
 	struct sigcontext uc_mcontext;
 	sigset_t          uc_sigmask;
} sig_ucontext_t;

int osType = 0;

void trap(int val){
        emu->AX = emu->EAX;
	    emu->AL = emu->EAX;
        emu->AH = emu->EAX;
        emu->DX = emu->EDX;
        emu->DL = emu->EDX;
        emu->DH = emu->EDX;
        emu->CX = emu->ECX;
        emu->CL = emu->ECX;
        emu->CH = emu->ECX;

        //like irq hardware polling
	    pic->chk_irq(emu);

	    //exec INT xx instruction
        inter->exec_interrupt(pic, emu);


		instruction_func_t* func;
		void * sp = __builtin_frame_address(0);
		struct _sig_ucontext *context = (struct _sig_ucontext*)(sp + 8);
		uint8_t * pc = (uint8_t*)context->uc_mcontext.rip;
		printf("opecode : %x\n", *pc);
		
		func = instructions16[__builtin_bswap64(*pc)];


#ifndef QUIET
		cout<<"emu: ";
		cout<<"EIP = "<<hex<<showbase<<emu->EIP<<", ";
		cout<<"Code = "<<(uint32_t)emu->instr.opcode<<endl;
#endif
		if(func == NULL){
			cout<<"命令("<<showbase<<(int)emu->instr.opcode<<")は実装されていません。"<<endl;
		}

		//execute
		func(emu);

		
		if(emu->EIP > emu->GetMemSize()){
			cout<<"out of memory."<<endl;
		}
	
	//emu->DumpRegisters(32);
	//emu->DumpMemory("memdump.bin");
}

int main(int argc, char **argv){

	 
	int opt;

	opterr = 1;

	bool hypervisor = false;
	while ((opt = getopt(argc, argv, "ho:")) != -1){
		switch (opt) {
			case 'o':
				if(*optarg == 'x'){
					osType = 1;
				} else if(*optarg == 'h'){
					osType = 0;
				}
				break;
			case 'h':
				hypervisor = true;
				break;
			default:
				printf("Usage: ./x86 ./path/to/img -o x|h \n");
				break;
		}
	}
	


if(hypervisor) {
	int p_id, status;
	// プロセスの生成
    if ((p_id = fork()) == 0) {
        cout << "プロセス生成" << endl;

		emu = new Emulator();
    	pic = new PIC();
    	//kb = new keyboard();

    	inter = new Interrupt();
		cout<<"emulator created."<<endl;

    	emu->LoadBinary("../xv6-public/xv6.img", 0x7c00, 1024 );
		printf("emu->memory : %x\n", *(emu->memory + 0x7c00));

		printf("emu->memory : %p\n", emu->memory);
		signal(SIGSEGV, trap);

		long i = 0x1000;
		_pc((uintptr_t)emu->memory, 0x7c00);
        exit(EXIT_SUCCESS);
    }


    // 親の処理
    wait(&status); // 子プロセス全部終わるまで待つ
    exit(EXIT_FAILURE);
}
if(osType == 0) { //hariboteOS
	emu = new Emulator();
    pic = new PIC();
    kb = new keyboard();

    inter = new Interrupt();
	cout<<"emulator created."<<endl;
	
	//disp = new Display(emu->memory + VRAM_ADDR);
	//gui = new GUI(disp);


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


	emu->EIP = 0x1b;
	emu->ESP = dest;


	//set vmode scrnx scrny vram
	emu->memory[0xff2] = 8;
	emu->SetMemory16(0xff4, 320);
    emu->SetMemory16(0xff6, 200);
	emu->SetMemory32(0xff8, 0xa0000);
//	emu->memory[0xff8] = 0xa0;

	gui->OpenWindow();
	
//	emu->ESP = 0xffff;
//	emu->DumpRegisters(32);
	//emulation
	emu->set_portio(0x20, 2, pic);
    emu->set_portio(0x60, 1, kb);
    emu->set_portio(0x60, 12, kb);

} else if(osType == 1){ //xv6
	emu = new Emulator();
    pic = new PIC();
    //kb = new keyboard();

    inter = new Interrupt();
	cout<<"emulator created."<<endl;
	
	//disp = new Display(emu->memory + 0xA0000);
	//gui = new GUI(disp);



	//BIOS
	//set_dram
    emu->LoadBinary("../xv6-public/xv6.img", 0x7c00, 1024 * 1024);

	//set_pc
	emu->EIP = 0x7c00;
	emu->sgregs[1].base = 0x0;
}

	for(size_t i = 0; true; i++){
        emu->AX = emu->EAX;
	    emu->AL = emu->EAX;
        emu->AH = emu->EAX;
        emu->DX = emu->EDX;
        emu->DL = emu->EDX;
        emu->DH = emu->EDX;
        emu->CX = emu->ECX;
        emu->CL = emu->ECX;
        emu->CH = emu->ECX;

        //like irq hardware polling
	    pic->chk_irq(emu);

	    //exec INT xx instruction
        inter->exec_interrupt(pic, emu);

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
 		if(osType == 0) {
			if(emu->instr.prefix) {
				func = instructions32[emu->instr.opcode];
			}else {
				func = instructions16[emu->instr.opcode];
			}
		} else if(osType == 1) {
			if(emu->instr.prefix && emu->is_16mode) {
				func = instructions16[emu->instr.opcode];
			}else {
				func = instructions32[emu->instr.opcode];
			}
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
