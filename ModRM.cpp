#include <iostream>

#include "ModRM.h"
#include "Emulator.h"

using namespace std;


ModRM::ModRM(Emulator *emu){
	this->emu = emu;
	Parse(emu);
}


void ModRM::Parse(Emulator *emu){	//cout<<"parse"<<endl;
	uint8_t code = emu->GetCode8(0);
	cout << "code=" << (uint32_t) code << endl;
	emu->instr.Mod = ((code & 0xC0) >> 6);
	emu->instr.opecode = ((code & 0x38) >> 3);
	emu->instr.RM = code & 0x07;

	cout << "Mod:" << (uint32_t) emu->instr.Mod << " RM:" << (uint32_t) emu->instr.RM << endl;

	emu->EIP++;

	if (emu->instr.Mod !=3 && emu->instr.RM == 4){
		emu->instr._SIB = emu->GetCode8(0);
	}

    if(emu->instr.prefix) {
		if (emu->instr.Mod != 3 && emu->instr.RM == 4) {
			emu->instr.SIB = emu->GetCode8(0);
			emu->EIP++;
		}

		if ((emu->instr.Mod == 0 && emu->instr.RM == 5) || emu->instr.Mod == 2) {
			emu->instr.disp32 = emu->GetSignCode32(0);
			emu->EIP += 4;
		} else if (emu->instr.Mod == 1) {
			emu->instr.disp8 = emu->GetSignCode8(0);
			emu->EIP++;
		}
	}else {
    	if((emu->instr.Mod == 0 && emu->instr.RM == 6) || emu->instr.Mod == 2){
			emu->instr.disp16 = emu->GetSignCode32(0);
			emu->EIP += 2;
    	}else if (emu->instr.Mod == 1){
			emu->instr.disp8 = emu->GetSignCode8(0);
			emu->EIP++;
    	}
    }

	return;
}

uint8_t ModRM::GetRM8(Emulator *emu){
	if(emu->instr.Mod == 3){
		return emu->GetRegister8(emu->instr.RM);
	}else{
		uint32_t addr = CalcMemAddr();
		return emu->GetMemory8(addr);
	}
}

uint8_t ModRM::GetRM8(){
	if(emu != NULL)	return GetRM8(emu);
}

void ModRM::SetRM8(Emulator *emu, uint8_t val){
	if(emu->instr.Mod == 3){
		emu->SetRegister8(emu->instr.RM, val);
	}else{
		uint32_t addr = CalcMemAddr();
		emu->SetMemory8(addr, val);
	}
}

void ModRM::SetRM8(uint8_t val){
	if(emu != NULL) SetRM8(emu, val); return;
}
void ModRM::SetRM16(Emulator *emu, uint16_t val){
	if(emu->instr.Mod == 3){
		emu->SetRegister16(emu->instr.RM, val);
	}else{
		uint32_t addr = CalcMemAddr();
		emu->SetMemory16(addr, val);
	}
}

void ModRM::SetRM16(uint16_t val){
	if(emu != NULL) SetRM16(emu, val); return;
}

uint16_t ModRM::GetRM16(Emulator *emu){
	if(emu->instr.Mod == 3){
		return emu->GetRegister16(emu->instr.RM);
	}else{
		uint32_t addr = CalcMemAddr();
		return emu->GetMemory16(addr);
	}
}

uint16_t ModRM::GetRM16(){
	if(emu != NULL)	return GetRM16(emu);
}

uint32_t ModRM::GetRM32(Emulator *emu){
	if(emu->instr.Mod == 3){
		return emu->GetRegister32(emu->instr.RM);
	}else{
		uint32_t addr = CalcMemAddr();
		return emu->GetMemory32(addr);
	}
}

uint32_t ModRM::GetRM32(){
	if(emu != NULL)	return GetRM32(emu);	
}

void ModRM::SetRM32(Emulator *emu, uint32_t val){
	if(emu->instr.Mod == 3){
		emu->SetRegister32(emu->instr.RM, val);
	}else{
		uint32_t addr = CalcMemAddr();
		emu->SetMemory32(addr, val);
		cout<<"setrm addr="<<addr;
		cout<<" val="<<val<<hex;
		cout<<" Mod="<<(uint32_t)emu->instr.Mod;
		cout<<" RM="<<(uint32_t)emu->instr.RM;
		cout<<endl;
	}
}

void ModRM::SetRM32(uint32_t val){
	if(emu != NULL)
		SetRM32(emu, val);
}

uint8_t ModRM::GetR8(Emulator *emu){
	return emu->GetRegister8(emu->instr.reg_index);
}

uint8_t ModRM::GetR8(){
	if(emu != NULL) return GetR8(emu);
}

uint16_t ModRM::GetR16(Emulator *emu){
	return emu->GetRegister16(emu->instr.reg_index);
}

uint16_t ModRM::GetR16(){
	if(emu != NULL) return GetR8(emu);
}

void ModRM::SetR8(Emulator *emu, uint8_t val){
	emu->SetRegister8(emu->instr.reg_index, val);
	if(emu->instr.reg_index == 0){
		emu->EAX = (emu->EAX & ~0xff) | emu->AL;
	}
}

uint16_t ModRM::SetR16(uint16_t val){
	if(emu != NULL) return SetR16(emu, val);
}

uint16_t ModRM::SetR16(Emulator *emu, uint16_t val) {
	emu->SetRegister16(emu->instr.reg_index, val);
}

void ModRM::SetR8(uint8_t val){
	if(emu != NULL)	return SetR8(emu, val);
}

uint32_t ModRM::GetR32(Emulator *emu){
	return emu->GetRegister32(emu->instr.reg_index);
}

uint32_t ModRM::GetR32(){
	if(emu != NULL)	return GetR32(emu);
}

void ModRM::SetR32(Emulator *emu, uint32_t val){
    printf("reg_index : %x, val : %x", emu->instr.reg_index, val);
	emu->SetRegister32(emu->instr.reg_index, val);
}

void ModRM::SetR32(uint32_t val){
	if(emu != NULL){
		SetR32(emu, val);
	}
}

void ModRM::SetTR(uint16_t sel){
    uint32_t gdt_base = emu->dtregs[GDTR].base_addr;
    uint16_t gdt_limit = emu->dtregs[GDTR].table_limit;
    TSSDesc tssdesc;

    emu->read_data(&tssdesc, gdt_base + sel, sizeof(TSSDesc));

	uint32_t base = (tssdesc.base_h << 24) + (tssdesc.base_m << 16) + tssdesc.base_l;
    uint16_t limit = (tssdesc.limit_h << 16) + tssdesc.limit_l;

	emu->set_dtreg(TR, sel, base, limit);
}

uint32_t ModRM::CalcMemAddr32(Emulator *emu){
	if(emu->instr.Mod == 0){
		if(emu->instr.RM == 4){
			return calc_sib();
		}else if(emu->instr.RM == 5){
			return emu->instr.disp32;
		}else{
			return emu->GetRegister32(emu->instr.RM);
		}
	}else if(emu->instr.Mod == 1){
		if(emu->instr.RM == 4){
			return emu->instr.disp8 + calc_sib();
		}else{
			return emu->GetRegister32(emu->instr.RM) + emu->instr.disp8;
		}
	}else if(emu->instr.Mod == 2){
		if(emu->instr.RM == 4){
			return emu->instr.disp32 + calc_sib();
		}else{
			return emu->GetRegister32(emu->instr.RM) + emu->instr.disp32;
		}
	}else{
		cout<<"not implemented ModRM Mod = 3"<<endl;
		return 0;
	}
}

uint32_t ModRM::CalcMemAddr16(Emulator *emu) {
	uint32_t addr = 0;

	switch(emu->instr.Mod){
		case 1:
			addr += emu->instr.disp8;
			break;
		case 2:
			addr += emu->instr.disp16;
	}

	switch(emu->instr.RM){
		case 4:
			addr += calc_sib();
			emu->EIP++;
		case 7:
			addr += emu->reg[3].reg16;
			break;
		case 6:
			if(emu->instr.Mod == 0 && emu->instr.RM == 6){
				addr += emu->instr.disp16;
			}else {
				addr += emu->reg[5].reg16;
				emu->instr.SEGMENT = 2;
			}
	}

	if(emu->instr.RM < 6){
	    if(emu->instr.RM % 2)
			addr += emu->reg[7].reg16;
	    else
			addr += emu->reg[6].reg16;
	}

	return addr;
}

uint32_t ModRM::calc_sib(void){
    uint32_t base;

    if(emu->instr.sib.base == 5 && emu->instr.Mod == 0)
        base = emu->instr.disp32;
    else if (emu->instr.sib.base == 4) {
        if(emu->instr.sib.scale == 0){
            emu->instr.SEGMENT = 2;
            base = 0;
        }
        else
            printf("not implemented SIB (base = %d, index = %d, scale = %d)\n", emu->instr.sib.base, emu->instr.sib.index, emu->instr.sib.scale);
    }
    else{
        emu->instr.SEGMENT = (emu->instr.RM == 5) ? 2 : 3;
        base = emu->reg[emu->instr.sib.base].reg32;
    }

    return base + emu->reg[emu->instr.sib.index].reg32 * (1<<emu->instr.sib.scale);
}

uint32_t ModRM::CalcMemAddr(){
	if(emu == NULL)
		return 0;

	if (emu->instr.prefix) {
	    uint32_t h = CalcMemAddr32(emu);
		return CalcMemAddr32(emu);
	} else {
		return CalcMemAddr16(emu);
	}
}

uint32_t ModRM::get_m(void){
	return CalcMemAddr();
}
