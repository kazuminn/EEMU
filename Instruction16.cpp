//
// Created by emu on 19/06/06.
//

#include "Emulator.h"
#include "ModRM.h"

instruction_func_t* instructions16[0xffff];

namespace instruction16{

void mov_r8_imm8(Emulator *emu){
	uint8_t reg = emu->GetCode8(0) - 0xB0;
	emu->reg[reg].reg16 = emu->GetCode8(1);
	emu->EIP += 2;
}

void add_rm16_r16(Emulator *emu) {
	emu->EIP++;
	ModRM modrm(emu);
	uint16_t rm16 = modrm.GetR16();
	uint16_t r16 = modrm.GetR16();

	modrm.SetRM16(rm16 + r16);
	emu->update_eflags_add(rm16, r16);
}

void mov_r16_rm32(Emulator *emu){
	printf("prefix 66 \n");
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t rm16 = modrm.GetRM16();
	modrm.SetR16(rm16);
}

void mov_rm16_r16(Emulator *emu){
    printf("prefix 66 \n");
	emu->EIP++;
	ModRM modrm(emu);
	uint16_t r16 = modrm.GetR16();
	modrm.SetRM16(r16);
}

void mov_rm16_sreg(Emulator *emu){
    emu->EIP++;
    ModRM modrm(emu);
    uint16_t sreg = emu->sreg[emu->instr.reg_index].sreg;
    modrm.SetRM16(sreg);
}

}

using namespace instruction16;

void InitInstructions16(){
	int i;
	instruction_func_t** func = instructions16;


//	func[0x00] = test;

	func[0x01] = add_rm16_r16;
	func[0x89] = mov_rm16_r16;
	func[0x8B] = mov_r16_rm32;
    func[0x8C] = mov_rm16_sreg;
	for(i=0;i<8;i++){
		func[0xB8 + i] = mov_r8_imm8;
	}
}

