#include "Emulator.h"
#include "ModRM.h"
#include "Instruction.h"

instruction_func_t* instructions16[256];

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

void mov_rm32_r16(Emulator *emu){
    emu->EIP++;
	ModRM modrm(emu);
	uint16_t r16 = modrm.GetR16();
	modrm.SetRM32((uint32_t)r16);
}

}

using namespace instruction16;

void InitInstructions16(){
	int i;
	instruction_func_t** func = instructions16;
	instruction::Init();


//	func[0x00] = test;

	func[0x01] = add_rm16_r16;
	func[0x89] = mov_rm32_r16;
	for(i=0;i<8;i++){
		func[0xB8 + i] = mov_r8_imm8;
	}
}


