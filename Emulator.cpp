#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <string>
#include <sstream>
#include "Emulator.h"
#include "ModRM.h"

using namespace std;

const char* registers_name16[] = { "AX",  "CX",  "DX",  "BX",  "SP",  "BP",  "SI",  "DI"};
const char* registers_name32[] = {"EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI"};


//void test(Emulator *emu){
//	cout<<"test("<<emu<<")"<<endl;
//}

Emulator::Emulator(){
	BitMode = DEFAULT_BIT_MODE;
	memory_size = 32 ;
	memory = new (nothrow) uint8_t[memory_size];
	if(memory == NULL){
		cout<<"error new."<<endl;
	}
	
	InitRegisters();

	sgregs[1].base = 0x280000; //1 == cs
	sgregs[3].base = 0x000000; //3 == DS
	sgregs[0].base = 0x000000; //0 == Es
	sgregs[2].base = 0x000000; //2 == Ss

	InitInstructions16();

	InitInstructions32();
	
//	memset(memory, 0x01, memory_size);
//	test(this);
}

Emulator::~Emulator(){
	delete[] memory;
}

int Emulator::parse_prefix(Emulator *emu){
	while(true) {
		uint8_t code = emu->GetSignCode8(0);
		switch(code) {
			case 0x66:
				emu->EIP++;
				return 0;
		}

		break;
	}
	return -1;
}

size_t Emulator::read_data(void *dst, uint32_t src_addr, size_t size){
    memcpy(dst, &memory[src_addr], size);
    return size;
}

void Emulator::set_dtreg(enum dtreg_t n, uint16_t sel, uint32_t base, uint16_t limit){
	dtregs[n].selector = sel;
	dtregs[n].base_addr = base;
	dtregs[n].table_limit = limit;

}

void Emulator::set_gdtr(uint32_t base, uint16_t limit){
	set_dtreg(GDTR, 0, base, limit);
}

//各レジスタの初期化
void Emulator::InitRegisters(){
	dtregs[GDTR].base_addr	= 0x0000;
	dtregs[GDTR].table_limit= 0xffff;	// GDTRの初期値はIntel*3巻2.4.1参照
	dtregs[IDTR].base_addr	= 0x0000;
	dtregs[IDTR].table_limit= 0xffff;	// 同上2.4.3
}

void Emulator::LoadBinary(const char* fname, uint32_t addr, int size){
	FILE *fp;
	fp = fopen(fname, "rb");
	if(fp == NULL){
		return;
	}
	fread(memory + addr, 1, size, fp);
	fclose(fp);
}

int Emulator::GetBitMode(){
	return BitMode;
}

uint8_t Emulator::GetCode8(int index){
	if(memory_size < (EIP +index)){
		cout<<"error"<<endl;
		return 0xff;
	}
	return memory[sgregs[1].base + EIP + index];
}

int8_t Emulator::GetSignCode8(int index){
	return (int8_t)GetCode8(index);
}

uint32_t Emulator::GetCode32(int index){
	uint32_t ret = 0;
	
	//リトルエンディアンでメモリの値を取得
	for(int i=0; i<4; i++){
		ret |= GetCode8(index + i) << (i * 8);
	}
	
	return ret;
}

int32_t Emulator::GetSignCode32(int index){
	return (int32_t)GetCode32(index);
}

uint8_t Emulator::GetRegister8(int index){
	if(index < 4){
		return reg[index].low8;
	}else{
		return reg[index - 4].high8;
	}
}

uint16_t Emulator::GetRegister16(int index){
	return reg[index].reg16;
}

uint32_t Emulator::GetRegister32(int index){
	return reg[index].reg32;
}

void Emulator::SetRegister8(int index, uint8_t val){
	if(index < 4){
		reg[index].low8 = val;
	}else{
		reg[index - 4].high8 = val;
	}
}

void Emulator::SetRegister16(int index, uint16_t val){
	reg[index].reg16 = val;
}

void Emulator::io_out8(uint16_t, uint8_t) {

}

uint16_t Emulator::in_io8(uint16_t) {
}


void Emulator::set_interrupt(bool interrupt){
	eflags.IF = interrupt;
}

void Emulator::SetRegister32(int index, uint32_t val){
	reg[index].reg32 = val;
}

uint32_t Emulator::GetMemory8(uint32_t addr){
	if(addr > memory_size){
		cout<<"fatal error:"<<"メモリサイズを超えたアドレス"<<addr<<"を参照しようとしました。"<<endl;
		return 0x00;
	}
	return memory[addr];
}

uint32_t Emulator::GetMemory16(uint32_t addr){
	// little endian
	uint32_t ret = 0;
	for(int i=0; i<2; i++){
		ret |= GetMemory8(addr + i) << (8 * i);
	}

	return ret;
}

uint32_t Emulator::GetMemory32(uint32_t addr){
	// little endian
	uint32_t ret = 0;
	for(int i=0; i<4; i++){
		ret |= GetMemory8(addr + i) << (8 * i);
	}
	
	return ret;
}

void Emulator::SetMemory8(uint32_t addr, uint32_t val){
	if(addr > memory_size){
		cout<<"fatal error:"<<"メモリサイズを超えたアドレス"<<addr<<"に値("<<(val & 0xff)<<")をセットしようとしました"<<endl;
		return;
	}
	cout<<addr<<"への書き込み("<<(val&0xff)<<endl;
	memory[addr] = val & 0xFF;
	return;
}

void Emulator::SetMemory16(uint32_t addr, uint32_t val){
	//little endian
	for(int i=0; i<2; i++){
		SetMemory8(addr + i, val >> (i*8));
	}

	return;
}

void Emulator::SetMemory32(uint32_t addr, uint32_t val){
	//little endian
	for(int i=0; i<4; i++){
		SetMemory8(addr + i, val >> (i*8));
	}

	return;
}

void Emulator::Push32(uint32_t val){
//	uint32_t addr = /*GetRegister32(ESP)*/ ESP  - 4;
//	SetRegister32(ESP, addr);
//	ESP = addr;
	ESP = ESP - 4;
	SetMemory32(ESP, val);
}

uint32_t Emulator::Pop32(){
	uint32_t addr = ESP;//GetRegister32(ESP);
	uint32_t ret = GetMemory32(addr);
//	SetRegister32(ESP, addr + 4);
	ESP = addr + 4;
	return ret;
}

void Emulator::DumpRegisters(int bit){
	cout<<"---Dump Registers---"<<endl;
	
	switch(bit){
	case 16:
		for(int i=0; i<REGISTERS_COUNT; i++){
			cout<<registers_name16[i]<<" = ";
			cout<<hex<<showbase<<reg[i].reg16;
			cout<<endl;
		}
		cout<<"IP = "<<IP<<endl;
		cout<<"FLAGS = "<<FLAGS<<endl;
		break;
	case 32:
		for(int i=0; i<REGISTERS_COUNT; i++){
			cout<<registers_name32[i]<<" = ";
			cout<<hex<<showbase<<reg[i].reg32;
			cout<<endl;
		}
		cout<<"EIP = "<<EIP<<endl;
		cout<<"EFLAGS = "<<EFLAGS<<endl;
		for(int i=0; i<4; i++){
			cout<<"CR"<<dec<<i<<" = ";
			cout<<hex<<showbase<<CR[i].reg32<<endl;
		}
		break;
	//case 64:
	//	break;
	default:
		break;
	}
	
	cout<<"--------------------"<<endl;
	
	return;
}

void Emulator::DumpRegisters(){
	int bit = GetBitMode();
	DumpRegisters(bit);
	return;
}

void Emulator::DumpMemory(const char *fname, uint32_t addr, uint32_t size){
	if(memory_size < sgregs[1].base + addr)
		return;
	if(memory_size < sgregs[1].base + addr+size){
		size = memory_size - sgregs[1].base + addr;
	}
	
	FILE *fp;
	fp = fopen(fname, "wb");
	fwrite(memory+addr, sizeof(char), size, fp);
	fclose(fp);
}



