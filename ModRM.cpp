#include <iostream>

#include "ModRM.h"

using namespace std;

#define DEBUG() cout<<"Mod:"<<(uint32_t)Mod<<" RM:"<<(uint32_t)RM<<endl;

ModRM::ModRM(){
	
}

ModRM::ModRM(Emulator *emu){
	this->emu = emu;
	Parse(emu);
}

uint32_t ModRM::calc_modrm32(void) {
	uint32_t addr = 0;

	switch(Mod){
		case 1:
			addr += disp8;
			break;
		case 2:
			addr += disp32;
			break;
	}

	switch(RM){
		case 4:
			addr += calc_sib();
		case 5:
		    if(Mod == 0) {
				addr += disp32;
				break;
			}
		default:
		    SEGMENT = (RM == 5) ? 2:3;
		    addr += emu->reg[RM].reg32;
	}
	return addr;
}

uint32_t ModRM::calc_sib(void){
    uint32_t base;

    if(sib.base == 5 && Mod == 0)
    	base = disp32;
    else if (sib.base == 4) {
    	if(sib.scale == 0){
    		SEGMENT = 2;
    		base = 0;
    	}
		else
			printf("not implemented SIB (base = %d, index = %d, scale = %d)\n", sib.base, sib.index, sib.scale);
	}
	else{
		SEGMENT = (RM == 5) ? 2 : 3;
		base = emu->reg[sib.base].reg32;
	}

	return base + emu->reg[sib.index].reg32 * (1<<sib.scale);
}

uint8_t ModRM::GetRM8(Emulator *emu){
	if(Mod == 3){
		return emu->GetRegister8(RM);
	}else{
		uint32_t addr = CalcMemAddr(emu);
		return emu->GetMemory8(addr);
	}
}

uint8_t ModRM::GetRM8(){
	if(emu != NULL)	return GetRM8(emu);
}

void ModRM::SetRM8(Emulator *emu, uint8_t val){
	if(Mod == 3){
		emu->SetRegister8(RM, val);
	}else{
		uint32_t addr = CalcMemAddr(emu);
		emu->SetMemory8(addr, val);
	}
}

void ModRM::SetRM8(uint8_t val){
	if(emu != NULL) SetRM8(emu, val); return;
}
void ModRM::SetRM16(Emulator *emu, uint16_t val){
	if(Mod == 3){
		emu->SetRegister16(RM, val);
	}else{
		uint32_t addr = CalcMemAddr(emu);
		emu->SetMemory16(addr, val);
	}
}

void ModRM::SetRM16(uint16_t val){
	if(emu != NULL) SetRM16(emu, val); return;
}

uint16_t ModRM::GetRM16(Emulator *emu){
	if(Mod == 3){
		return emu->GetRegister16(RM);
	}else{
		uint32_t addr = CalcMemAddr(emu);
		return emu->GetMemory16(addr);
	}
}

uint16_t ModRM::GetRM16(){
	if(emu != NULL)	return GetRM16(emu);
}

uint32_t ModRM::GetRM32(Emulator *emu){
	if(Mod == 3){
		return emu->GetRegister32(RM);
	}else{
		uint32_t addr = CalcMemAddr(emu);
		return emu->GetMemory32(addr);
	}
}

uint32_t ModRM::GetRM32(){
	if(emu != NULL)	return GetRM32(emu);	
}

void ModRM::SetRM32(Emulator *emu, uint32_t val){
	if(Mod == 3){
		emu->SetRegister32(RM, val);
	}else{
		uint32_t addr = CalcMemAddr(emu);
		emu->SetMemory32(addr, val);
		cout<<"setrm addr="<<addr;
		cout<<" val="<<val<<hex;
		cout<<" Mod="<<(uint32_t)Mod;
		cout<<" RM="<<(uint32_t)RM;
		cout<<endl;
	}
}

void ModRM::SetRM32(uint32_t val){
	if(emu != NULL)
		SetRM32(emu, val);
}

uint8_t ModRM::GetR8(Emulator *emu){
	return emu->GetRegister32(reg_index);
}

uint8_t ModRM::GetR8(){
	if(emu != NULL) return GetR8(emu);
}

uint16_t ModRM::GetR16(Emulator *emu){
	return emu->GetRegister16(reg_index);
}

uint16_t ModRM::GetR16(){
	if(emu != NULL) return GetR8(emu);
}

void ModRM::SetR8(Emulator *emu, uint8_t val){
	emu->SetRegister8(reg_index, val);
}

void ModRM::SetR8(uint8_t val){
	if(emu != NULL)	return SetR8(val);
}

uint32_t ModRM::GetR32(Emulator *emu){
	return emu->GetRegister32(reg_index);
}

uint32_t ModRM::GetR32(){
	if(emu != NULL)	return GetR32(emu);
}

void ModRM::SetR32(Emulator *emu, uint32_t val){
	emu->SetRegister32(reg_index, val);
}

void ModRM::SetR32(uint32_t val){
	if(emu != NULL){
		SetR32(emu, val);
	}
}

uint32_t ModRM::CalcMemAddr(Emulator *emu){
	if(Mod == 0){
		if(RM == 4){
			cout<<"not implemented ModRM Mod = 0, RM = 4"<<endl;
			return 0;
		}else if(RM == 5){
			return disp32;
		}else{
			return emu->GetRegister32(RM);
		}
	}else if(Mod == 1){
		if(RM == 4){
			cout<<"not implemented ModRM Mod = 1, RM = 4"<<endl;
			return 0;
		}else{
			return emu->GetRegister32(RM) + disp8;
		}
	}else if(Mod == 2){
		if(RM == 4){
			cout<<"not implemented ModRM Mod = 2, RM = 4"<<endl;
			return 0;
		}else{
			return emu->GetRegister32(RM) + disp32;
		}
	}else{
		cout<<"not implemented ModRM Mod = 3"<<endl;
		return 0;
	}
}

uint32_t ModRM::CalcMemAddr(){
	if(emu == NULL)
		return 0;

	return CalcMemAddr(emu);
}
