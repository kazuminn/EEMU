#include "Emulator.h"
#include "ModRM.h"

instruction_func_t* instructions16[256];

namespace instruction16{

void test(Emulator *emu){
	
}

void mov_r8_imm8(Emulator *emu){
	uint8_t reg = emu->GetCode8(0) - 0xB0;
	emu->reg[reg].reg16 = emu->GetCode8(1);
	emu->EIP += 2;
}

void add_rm16_r16(Emulator *emu) {
	ModRM modrm(emu);
	uint16_t rm16 = modrm.GetR16();
	uint16_t r16 = modrm.GetR16();

	modrm.SetRM16(rm16 + r16);
	emu->update_eflags_add(rm16, r16);
}

}

using namespace instruction16;

void InitInstructions16(){
	int i;
	instruction_func_t** func = instructions16;
	
//	func[0x00] = test;

	func[0x01] = add_rm16_r16;
	for(i=0;i<8;i++){
		func[0xB8 + i] = mov_r8_imm8;
	}
}


