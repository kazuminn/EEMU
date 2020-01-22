#include <iostream>
using namespace std;

//memo
//fprintf(stderr, "%x \n", emu->memory[0xff8]);
//if(emu->GetCode8(0) == 0x75){
//fprintf(stderr,  "%x : %x \n", emu->EIP, emu->ESI);
//}

#include "Emulator.h"
#include "ModRM.h"

instruction_func_t* instructions32[0xffff];

namespace instruction32{

// 圧倒的NOP
void nop(Emulator *emu){
	emu->EIP++;
}

void ltr_rm16(Emulator *emu, ModRM *modrm){
	uint16_t rm16 = modrm->GetRM16();
	emu->SetTR(rm16);
}

void lgdt_m32(Emulator *emu, ModRM *modrm) {
	uint32_t m48 = modrm->get_m();
	uint16_t limit = emu->GetMemory16(m48);
	uint32_t base = emu->GetMemory32(m48 + 2);

    emu->set_gdtr(base, limit);
}

void lidt_m32(Emulator *emu, ModRM *modrm) {
	uint32_t m48 = modrm->get_m();
	uint16_t limit = emu->GetMemory16(m48);
	uint32_t base = emu->GetMemory32(m48 + 2);

    emu->set_idtr(base, limit);
}

void push_es(Emulator *emu){
	emu->Push32(emu->get_ES());
	emu->EIP++;
}
void pop_es(Emulator *emu){
	emu->set_ES(emu->Pop32());
	emu->EIP++;
}

void push_ss(Emulator *emu){
	emu->Push32(emu->get_SS());
	emu->EIP++;
}

void pop_ss(Emulator *emu){
	emu->set_SS(emu->Pop32());
	emu->EIP++;
}

void push_ds(Emulator *emu){
    emu->Push32(emu->get_DS());
    emu->EIP++;
}

void pop_ds(Emulator *emu){
	emu->set_DS(emu->Pop32());
	emu->EIP++;
}

void add_rm32_r32(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t rm32 = modrm.GetRM32();
	uint32_t r32 = modrm.GetR32();
	modrm.SetRM32(rm32 + r32);
	emu->update_eflags_add(rm32, r32);
}

void add_r32_rm32(Emulator *emu){
    emu->EIP++;
    ModRM modrm(emu);
    uint32_t r32 = modrm.GetR32();
    uint32_t rm32 = modrm.GetRM32();
    modrm.SetR32(rm32 + r32);
	emu->update_eflags_add(r32, rm32);
}

void add_eax_imm32(Emulator *emu){
    uint32_t imm32 = emu->GetSignCode32(1);
    uint32_t eax = emu->EAX;
	emu->EAX = eax + imm32;
	emu->EIP += 5;
	emu->update_eflags_add(eax, imm32);
}

void add_rm32_imm32(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint32_t imm32 = (int32_t)emu->GetSignCode32(0);
	emu->EIP += 4;
	modrm->SetRM32(rm32 + imm32);
	emu->update_eflags_add(rm32, imm32);
}

//void add_al_imm8(Emulator *emu){
//	uint8_t	al = emu->AL;
//	uint8_t imm8 = emu->GetSignCode8(1);
//	emu->AL = al + imm8;
//}

void dec_r32(Emulator *emu){
	uint8_t reg = emu->GetSignCode8(0) & ((1<<3)-1);
	uint32_t r32 = emu->reg[reg].reg32;
	emu->reg[reg].reg32 = r32 - 1;
	emu->EIP++;
	emu->update_eflags_sub(r32, 1);
}

void mov_rm8_r8(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	uint8_t r8 = modrm.GetR8();
	modrm.SetRM8(r8);
}

void mov_r8_imm8(Emulator *emu){
	uint8_t reg = emu->GetCode8(0) - 0xB0;
	emu->reg[reg].reg32 = emu->GetCode8(1);
	emu->EIP += 2;
//	std::cout<<"a";
}

void mov_r32_imm32(Emulator *emu){
	uint8_t reg	= emu->GetCode8(0) - 0xB8;
	uint32_t val = emu->GetCode32(1);
	printf("reg %x \n", val);
	emu->reg[reg].reg32 = val;
	emu->EIP += 5;
}

void mov_rm8_imm8(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	uint8_t val = emu->GetCode8(0);
	modrm.SetRM8(val);
	emu->EIP++;
}

void mov_r8_rm8(Emulator *emu){//		cout<<"mov_r8_rm8"<<endl;
	emu->EIP++;
	ModRM modrm(emu);
	uint8_t rm8 = modrm.GetRM8();
	modrm.SetR8(rm8);
}

void out_dx_al(Emulator *emu){
	emu->io_out8(emu->DX, emu->AL);
	emu->EIP++;
}

void sti(Emulator *emu){
	emu->set_interrupt(true);
	emu->EIP++;
}

void mov_r32_rm32(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t rm32 = modrm.GetRM32();
	modrm.SetR32(rm32);
}

void inc_r32(Emulator *emu){
	uint8_t reg = emu->GetCode8(0) - 0x40;
	emu->reg[reg].reg32++;
	emu->EIP++;
}

void lea_r32_m32(Emulator *emu){
    emu->EIP++;
	ModRM modrm(emu);
    uint32_t m32 = modrm.get_m();
    modrm.SetR32(m32);
}

void mov_al_moffs8(Emulator *emu) {
	emu->EIP++;
	uint8_t moffs = emu->memory[emu->sgregs[1].base + emu->GetSignCode32(0)];
	emu->AL = moffs;
	emu->EAX = (emu->EAX & ~0xff) | emu->AL;
	emu->EIP += 4;
}


void add_rm32_imm8(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	int32_t imm8 = (int32_t)emu->GetSignCode8(0);
	if(imm8 >= 0x80){
		imm8 -= 0x100;
	}
	emu->EIP++;
	modrm->SetRM32(rm32 + imm8);
	emu->update_eflags_add(rm32, imm8);
}

void mov_rm32_imm32(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t imm32 = emu->GetCode32(0);
	emu->EIP+=4;
	modrm.SetRM32(imm32);		cout<<"mov val="<<imm32<<endl;
}

void mov_rm32_r32(Emulator *emu){	//cout<<"mov2"<<endl;
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t r32 = modrm.GetR32();
	modrm.SetRM32(r32);
}

/*
void in_al_dx(Emulator *emu){
	uint16_t addr);
	
}
*/
void short_jump(Emulator *emu){
    uint32_t diff = emu->GetSignCode8(1);
	emu->EIP += diff + 2;
}

void near_jump(Emulator *emu){
	int32_t diff = emu->GetSignCode32(1);
	emu->EIP += (diff + 5);
}

void mov_moffs32_eax(Emulator *emu){
	emu->EIP++;
    uint32_t moffs = emu->GetSignCode32(0);
	emu->SetMemory32(moffs, emu->EAX);
	emu->EIP += 4;
}

void in_al_dx(Emulator *emu){
	uint16_t dx = emu->DX;
	emu->AL = emu->in_io8(dx);
	emu->EAX = (emu->EAX & ~0xff) | emu->AL;
	emu->EIP++;
}

void pushfd(Emulator *emu){
    emu->Push32(emu->get_eflags());
    emu->EIP++;
}

void sub_rm32_imm32(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint32_t imm32 = (int32_t)emu->GetSignCode32(0);
	emu->EIP += 4;
	modrm->SetRM32(rm32 - imm32);
	emu->update_eflags_sub(rm32, imm32);
}
void push_imm32(Emulator *emu) {
	uint32_t imm32 = (int32_t)emu->GetSignCode32(1);
	emu->Push32(imm32);
	emu->EIP += 5;
}

void sub_rm32_imm8(Emulator *emu, ModRM *modrm){
    uint32_t rm32 = modrm->GetRM32();
    int32_t imm8 = (int32_t)emu->GetSignCode8(0);
	if(imm8 >= 0x80){
		imm8 -= 0x100;
	}
    emu->EIP++;
    modrm->SetRM32(rm32 - imm8);
	emu->update_eflags_sub(rm32, imm8);
}


void sub_rm32_r32(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t rm32 = modrm.GetRM32();
	uint32_t r32 = modrm.GetR32();
	modrm.SetRM32(rm32 - r32);
	emu->update_eflags_sub(rm32, r32);
}

void sub_r32_rm32(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t r32 = modrm.GetR32();
	uint32_t rm32 = modrm.GetRM32();
	modrm.SetR32(rm32 - r32);
	emu->update_eflags_sub(r32, rm32);
}

void sub_eax_imm32(Emulator *emu) {
    uint32_t imm32 = emu->GetSignCode32(1); uint32_t eax = emu->reg[0].reg32;
	emu->reg[0].reg32 = emu->reg[0].reg32 - imm32;
	emu->EIP += 5;
	emu->update_eflags_sub(eax , imm32);
}

void xor_rm32_r32(Emulator *emu) {
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t rm32 = modrm.GetRM32();
	uint32_t r32 = modrm.GetR32();
	modrm.SetRM32(r32 ^ rm32);
	emu->UpdateXor();
}

void xor_r32_rm32(Emulator *emu) {
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t r32 = modrm.GetR32();
	uint32_t rm32 = modrm.GetRM32();
	modrm.SetR32(r32 ^ rm32);
	emu->UpdateXor();
}

void xor_rm32_imm32(Emulator *emu, ModRM *modrm) {
	uint32_t rm32 = modrm->GetRM32();
	uint32_t imm32 = emu->GetSignCode32(0);
	emu->EIP += 4;
	modrm->SetRM32(imm32 ^ rm32);
	emu->UpdateXor();
}

void xor_rm32_imm8(Emulator *emu, ModRM *modrm) {
	uint32_t rm32 = modrm->GetRM32();
	int32_t imm8 = (int32_t)emu->GetSignCode8(0);
	if(imm8 >= 0x80){
		imm8 -= 0x100;
	}
	emu->EIP++;
	modrm->SetRM32(imm8 ^ rm32);
	emu->UpdateXor();
}

void xor_eax_imm32(Emulator *emu) {
    emu->reg[0].reg32 = emu->reg[0].reg32 ^ emu->GetSignCode32(1);
    emu->EIP += 5;
	emu->UpdateXor();
}

void or_rm32_imm32(Emulator *emu, ModRM *modrm) {
	uint32_t rm32 = modrm->GetRM32();
	uint8_t imm32 = emu->GetSignCode32(0);
	emu->EIP += 4;
	modrm->SetRM32(imm32 | rm32);
	emu->UpdateOr();
}

void or_rm32_imm8(Emulator *emu, ModRM *modrm) {
	uint32_t rm32 = modrm->GetRM32();
	int32_t imm8 = (int32_t)emu->GetSignCode8(0);
	if(imm8 >= 0x80){
		imm8 -= 0x100;
	}
	emu->EIP += 1;
	modrm->SetRM32(imm8 | rm32);
	emu->UpdateOr();
}

void or_rm32_r32(Emulator *emu) {
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t rm32 = modrm.GetRM32();
	uint32_t r32 = modrm.GetR32();
	modrm.SetRM32(r32 | rm32);
	emu->UpdateOr();
}

void or_r32_rm32(Emulator *emu) {
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t r32 = modrm.GetR32();
	uint32_t rm32 = modrm.GetRM32();
	modrm.SetR32(r32 | rm32);
	emu->UpdateOr();
}
void or_eax_imm32(Emulator *emu) {
	emu->reg[0].reg32 = emu->reg[0].reg32 | emu->GetSignCode32(1);
	emu->EIP += 5;
	emu->UpdateOr();
}

void cmp_rm32_imm32(Emulator *emu, ModRM *modrm){
	uint32_t rm32= modrm->GetRM32();
	uint32_t imm32 = emu->GetCode32(0);
	emu->update_eflags_sub(rm32, imm32);
	emu->EIP += 4;
}

void cmp_rm32_imm8(Emulator *emu, ModRM *modrm){
	uint32_t rm32= modrm->GetRM32();
	int32_t imm8 = (int32_t)emu->GetCode8(0);
	if(imm8 >= 0x80){
		imm8 -= 0x100;
	}
	emu->update_eflags_sub(rm32, imm8);
	emu->EIP++;
}

void shl_rm32_imm8(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint32_t imm8 = emu->GetCode8(0);
	modrm->SetRM32(rm32<<imm8);
	emu->update_eflags_shl(rm32, imm8);
	emu->EIP++;
}

void shl_rm32_cl(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint8_t cl = emu->get_CL();
	modrm->SetRM32(rm32<<cl);
	emu->update_eflags_shl(rm32, cl);
}

void shr_rm32_imm8(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint32_t imm8 = emu->GetCode8(0);
	modrm->SetRM32(rm32>>imm8);
	emu->EIP++;
	emu->update_eflags_shr(rm32, imm8);
}

void shr_rm32_cl(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint8_t cl = emu->get_CL();
	modrm->SetRM32(rm32>>cl);
	emu->update_eflags_shr(rm32, cl);
}

void sal_rm32_imm8(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint32_t imm8 = emu->GetCode8(0);
	modrm->SetRM32(rm32<<imm8);
	//emu->update_eflags_shr(rm32, imm8);
}

void sal_rm32_cl(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint8_t cl = emu->get_CL();
	modrm->SetRM32(rm32<<cl);
	//emu->update_eflags_shr(rm32, cl);
}

void sar_rm32_imm8(Emulator *emu, ModRM *modrm){
	int32_t rm32 = modrm->GetRM32();
	int32_t imm8 = emu->GetCode8(0);
	modrm->SetRM32(rm32>>imm8);
	emu->EIP++;
	//emu->update_eflags_shr(rm32, imm8);
}

void sar_rm32_cl(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint8_t cl = emu->get_CL();
	modrm->SetRM32(rm32>>cl);
	//emu->update_eflags_shr(rm32, cl);
}

void sar_rm8_imm8(Emulator *emu, ModRM *modrm){
	int8_t rm8_s = modrm->GetRM8();
	uint8_t imm8 = emu->GetSignCode8(0);
	modrm->SetRM8(rm8_s>>imm8);
	emu->EIP += 1;
}

void shr_rm8_imm8(Emulator *emu, ModRM *modrm){
	uint8_t rm8_s = modrm->GetRM8();
	uint8_t imm8 = emu->GetSignCode8(0);
	modrm->SetRM8(rm8_s>>imm8);
	emu->EIP += 1;
}

void and_rm32_imm8(Emulator *emu, ModRM *modrm) {
	uint32_t rm32 = modrm->GetRM32();
	int32_t imm8 = (int32_t)emu->GetSignCode8(0);
	if(imm8 >= 0x80){
		imm8 -= 0x100;
	}
	modrm->SetRM32(rm32&imm8);
	emu->EIP++;
}

void cmp_rm8_imm8(Emulator *emu, ModRM *modrm) {
	uint8_t rm8 = modrm->GetRM8();
	uint8_t imm8 = emu->GetSignCode8(0);
	emu->update_eflags_sub(rm8, imm8);
	emu->EIP++;
}

void movzs_r32_rm8(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);
	uint8_t rm8 = modrm->GetRM8();
	modrm->SetR32(rm8);
}

void idiv_edx_eax_rm32(Emulator *emu, ModRM *modrm){
    int32_t rm32_s = modrm->GetRM32();
    int64_t val_s = emu->EDX;
    val_s <<= 32;
    val_s |= emu->EAX;

    emu->EAX = val_s/rm32_s;
    emu->EDX = val_s%rm32_s;
}

void test_rm8_imm8(Emulator *emu, ModRM *modrm){
	uint8_t	rm8 = modrm->GetRM8();
	uint8_t imm8 = emu->GetCode8(0);
	emu->EIP++;
	emu->update_eflags_and(rm8, imm8);
}

void and_rm32_imm32(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint32_t imm32 = emu->GetCode32(0);

	modrm->SetRM32(rm32&imm32);
	emu->update_eflags_add(rm32, imm32);
	emu->EIP += 4;
}

void cmp_al_imm8(Emulator *emu){
	uint8_t	al = emu->AL;
	uint8_t imm8 = emu->GetCode8(1);

	emu->update_eflags_sub(al, imm8);
	emu->EIP += 2;
}

void test_rm32_imm32(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint32_t imm32 = emu->GetCode32(0);

	emu->update_eflags_and(rm32, imm32);
	emu->EIP += 4;
}

void code_f6(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);

	switch(emu->instr.opecode){
		case 0: test_rm8_imm8(emu, modrm);  break;
		default:
			cout<<"not implemented: f6 "<<(uint32_t)emu->instr.opecode<<endl;
	}
	delete modrm;
}

void code_f7(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);

	switch(emu->instr.opecode){
		case 0: test_rm32_imm32(emu, modrm);  break;
		case 7: idiv_edx_eax_rm32(emu, modrm);  break;
		default:
			cout<<"not implemented: f7 "<<(uint32_t)emu->instr.opecode<<endl;
	}
	delete modrm;
}

void code_80(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);

	switch(emu->instr.opecode){
		case 7: cmp_rm8_imm8(emu, modrm);  break;
		default:
			cout<<"not implemented: 80 "<<(uint32_t)emu->instr.opecode<<endl;
	}
	delete modrm;
}

void code_81(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);

	switch(emu->instr.opecode){
		case 0: add_rm32_imm32(emu, modrm);  break;
		case 1: or_rm32_imm32(emu, modrm);   break;
		case 4: and_rm32_imm32(emu, modrm);  break;
		case 5: sub_rm32_imm32(emu, modrm);  break;
		case 6: xor_rm32_imm32(emu, modrm);  break;
		case 7: cmp_rm32_imm32(emu, modrm);  break;
		default:
			cout<<"not implemented: 81 "<<(uint32_t)emu->instr.opecode<<endl;
	}
	delete modrm;
}

void code_83(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);
	printf("REG :%x \n", emu->instr.opecode);

	switch(emu->instr.opecode){
		case 0: add_rm32_imm8(emu, modrm); break;
		case 1: or_rm32_imm8(emu, modrm);  break;
		case 4: and_rm32_imm8(emu, modrm);  break;
		case 5: sub_rm32_imm8(emu, modrm); break;
        case 6: xor_rm32_imm8(emu, modrm); break;
		case 7: cmp_rm32_imm8(emu, modrm); break;
		default:
			cout<<"not implemented: 83 "<<(uint32_t)emu->instr.opecode<<endl;
	}
	delete modrm;
}

void code_c0(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);

	switch(emu->instr.opecode){
		case 5: shr_rm8_imm8(emu, modrm); break;
		case 7: sar_rm8_imm8(emu, modrm); break;
		default:
			cout<<"not implemented: c0 "<<(uint32_t)emu->instr.opecode<<endl;
	}
	delete modrm;
}

void code_0f00(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);

	switch(emu->instr.opecode){
		case 3: ltr_rm16(emu, modrm);  break;
		default:
			cout<<"not implemented: 0f00 "<<(uint32_t)emu->instr.opecode<<endl;
	}
	delete modrm;
}

void code_c1(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);
	cout<<"opecode "<<(uint32_t)emu->instr.opecode<<endl;

	switch(emu->instr.opecode){
		case 4: shl_rm32_imm8(emu, modrm); break;
		case 5: shr_rm32_imm8(emu, modrm); break;
		case 6: sal_rm32_imm8(emu, modrm); break;
		case 7: sar_rm32_imm8(emu, modrm); break;
		default:
			cout<<"not implemented: c1 "<<(uint32_t)emu->instr.opecode<<endl;
	}
	delete modrm;
}

void code_d3(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);

	switch(emu->instr.opecode){
		case 4: shl_rm32_cl(emu, modrm); break;
		case 5: shr_rm32_cl(emu, modrm); break;
		case 6: sal_rm32_cl(emu, modrm); break;
		case 7: sar_rm32_cl(emu, modrm); break;
		default:
			cout<<"not implemented: d3 "<<(uint32_t)emu->instr.opecode<<endl;
	}
	delete modrm;
}

void code_0f01(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);

	switch(emu->instr.opecode){
		case 2: lgdt_m32(emu, modrm); break;
		case 3: lidt_m32(emu, modrm); break;
		default:
			cout<<"not implemented: 0f01 "<<(uint32_t)emu->instr.opecode<<endl;
	}
	delete modrm;
}

void push_r32(Emulator *emu){
    for(size_t i = 0; i< 8; i++){
        printf("hoge %x %x %x %x\n", emu->memory[emu->ESP + i * 4] ,emu->memory[emu->ESP + i * 4+ 1], emu->memory[emu->ESP + i * 4+ 2], emu->memory[emu->ESP + i* 4 + 3]);
    }
	uint8_t reg = emu->GetCode8(0) & ((1<<3)-1);
	emu->Push32(emu->GetRegister32(reg));
	emu->EIP++;
}

void pop_r32(Emulator *emu){
	uint8_t reg = emu->GetCode8(0) & ((1<<3)-1);
	uint32_t val = emu->Pop32();
	printf("%x \n", val);
	emu->SetRegister32(reg, val);
	emu->EIP++;
}

void push_imm8(Emulator *emu){
	uint32_t val = emu->GetCode8(1);
	printf("val %x \n", val);
	emu->Push32(val);
	emu->EIP += 2;
}

void inc_rm32(Emulator *emu, ModRM *modrm){
	uint32_t val = modrm->GetRM32();
	modrm->SetRM32(val + 1);
}

void push_rm32(Emulator *emu, ModRM *modrm){
	emu->Push32(modrm->GetRM32());
}

void dec_rm32(Emulator *emu, ModRM *modrm){
	uint32_t val = modrm->GetRM32();
	modrm->SetRM32(val - 1);
	emu->update_eflags_sub(val, 1);
}

void task_switch(Emulator *emu, uint16_t cs) {
    TSS oldTSS, newTSS;
    uint32_t base = emu->dtregs[TR].base_addr;
    fprintf(stderr, "%x \n", base);
    emu->read_data(&oldTSS, base, sizeof(TSS));//
    oldTSS.eflags = emu->get_eflags();
    oldTSS.eip = emu->EIP;
    oldTSS.eax = emu->EAX;
    oldTSS.ecx = emu->ECX;
    oldTSS.edx = emu->EDX;
    oldTSS.ebx = emu->EBX;
    oldTSS.esp = emu->ESP;
    oldTSS.ebp = emu->EBP;
    oldTSS.esi = emu->ESI;
    oldTSS.edi = emu->EDI;
    oldTSS.es = emu->sreg[0].sreg;
    oldTSS.cs = emu->sreg[1].sreg;
    oldTSS.ss = emu->sreg[2].sreg;
    oldTSS.ds = emu->sreg[3].sreg;
    oldTSS.fs = emu->sreg[4].sreg;
    oldTSS.gs = emu->sreg[5].sreg;
    emu->write_data(base, &oldTSS, sizeof(TSS));

    emu->SetTR(cs);
    base = emu->dtregs[TR].base_addr;
    emu->read_data(&newTSS, base, sizeof(TSS));//

    fprintf(stderr, "hoge1 \n");

    emu->set_eflags(newTSS.eflags);
    emu->EIP = newTSS.eip;
    emu->EAX = newTSS.eax;
    emu->ECX = newTSS.ecx;
    emu->EDX = newTSS.edx;
    emu->EBX = newTSS.ebx;
    emu->ESP = newTSS.esp;
    fprintf(stderr, "hoge \n");
    fprintf(stderr, "%x \n", newTSS.esp);
    fprintf(stderr, "%x \n", emu->ESP);
    emu->EBP = newTSS.ebp;
    emu->ESI = newTSS.esi;
    emu->EDI = newTSS.edi;
    emu->sreg[0].sreg = newTSS.es;
    emu->sreg[1].sreg = newTSS.cs;
    emu->sreg[2].sreg = newTSS.ss;
    emu->sreg[3].sreg = newTSS.ds;
    emu->sreg[4].sreg = newTSS.fs;
    emu->sreg[5].sreg = newTSS.gs;
    emu->set_ldtr(newTSS.ldtr);
    fprintf(stderr, "hoge2 \n");
}

void farjump(Emulator *emu, ModRM *modrm){
    uint32_t m48 = modrm->get_m();
    uint32_t eip = emu->GetMemory32(m48);
    uint16_t cs = emu->GetMemory16(m48 + 4);

    cs = cs & 0xfffc;
    if (cs == 0) {
       emu->EIP = 0;
    }

    uint16_t bit = (cs >> 2) & 1;
    if ( bit == 0){

    }else if (bit == 1) {
        emu->EIP = 0;
        //LDT
    }

    uint32_t smtype = emu->memory[emu->dtregs[GDTR].base_addr + cs + 5] & 0x9f;
    switch(smtype) {
        case 0x89:
        case 0x8b:
            task_switch(emu, cs);
            break;
        case 0x98:
        case 0x99:
        case 0x9a:
        case 0x9b:
        case 0x9c:
        case 0x9d:
        case 0x9e:
        case 0x9f:
            emu->EIP = eip;
            emu->sreg[1].sreg = cs;
            break;
    }

}


    void code_ff(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);
	
	switch(emu->instr.opecode){
	case 0: inc_rm32(emu, modrm); break;
    case 1: dec_rm32(emu, modrm); break;
    case 5: farjump(emu, modrm); break;
	case 6: push_rm32(emu, modrm);break;
	default:
		cout<<"not implemented: 0xFF /"<<(int)emu->instr.opecode<<endl;
	}
	
	delete modrm;
}

void movsx_r32_rm8(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	int8_t rm8_s = modrm.GetRM8();
	modrm.SetR32(rm8_s);
}

void call_rel32(Emulator *emu){
	int32_t diff = emu->GetSignCode32(1);
	printf("%x \n", emu->EIP);
	emu->Push32(emu->EIP + 5);	//call命令は全体で5バイト
	emu->EIP += (diff + 5);
}

void ret(Emulator *emu){//	cout<<"ret"<<endl;
	emu->EIP = emu->Pop32();
	printf("%x \n", emu->EIP);
}

void leave(Emulator *emu){
//	uint32_t ebp = EBP;
	emu->ESP = emu->EBP;
	emu->EBP = emu->Pop32();
	emu->EIP++;
}

void pushad(Emulator *emu) {
    uint32_t esp;
    esp = emu->ESP;

    emu->Push32(emu->EAX);
	emu->Push32(emu->ECX);
	emu->Push32(emu->EDX);
	emu->Push32(emu->EBX);
	emu->Push32(esp);
	emu->Push32(emu->EBP);
	emu->Push32(emu->ESI);
	emu->Push32(emu->EDI);

	emu->EIP++;
}

void popad(Emulator *emu) {
	uint32_t esp;

	emu->EDI = emu->Pop32();
	emu->ESI = emu->Pop32();
	emu->EBP = emu->Pop32();
	esp = emu->Pop32();
	emu->EBX = emu->Pop32();
	emu->EDX = emu->Pop32();
	emu->ECX = emu->Pop32();
	emu->EAX = emu->Pop32();

	emu->ESP = esp;
	emu->EIP++;
}
void jc(Emulator *emu){
	int diff = emu->IsCarry() ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}
void jz_rel8(Emulator *emu){
	int diff = emu->IsZero() ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}
void jz_rel32(Emulator *emu){
	int diff = emu->IsZero() ? emu->GetSignCode32(1) : 0;
	emu->EIP += (diff + 5);
}
void js(Emulator *emu){
	int diff = emu->IsSign() ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}
void jo(Emulator *emu){
	int diff = emu->IsOverflow() ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}
void jnc(Emulator *emu){
	int diff = !emu->IsCarry() ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}
void jnz(Emulator *emu){
	int diff = !emu->IsZero() ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}
void jns(Emulator *emu){
	int diff = !emu->IsSign() ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}
void jno(Emulator *emu){
	int diff = !emu->IsOverflow() ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}
void jbe(Emulator *emu){
	int diff = emu->IsCarry() || emu->IsZero() ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}

void jnl(Emulator *emu){
	int diff = emu->IsSign() == emu->IsOverflow() ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}

void jnle(Emulator *emu){
	int diff = (!emu->IsZero() && (emu->IsSign() == emu->IsOverflow())) ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}

void ja(Emulator *emu){
	int diff = !(emu->IsCarry() || emu->IsZero()) ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}

void jle_rel32(Emulator *emu){
	int diff = emu->IsZero() || (emu->IsSign() != emu->IsOverflow()) ? emu->GetSignCode32(1) : 0;
	emu->EIP += (diff + 5);
}

void js_rel32(Emulator *emu){
	int diff = emu->IsSign() ? emu->GetSignCode32(1) : 0;
	emu->EIP += (diff + 5);
}

void jnle_rel32(Emulator *emu){
	int diff = (!emu->IsZero() && (emu->IsSign() == emu->IsOverflow())) ? emu->GetSignCode32(1) : 0;
	emu->EIP += (diff + 5);
}

void jnz_rel32(Emulator *emu){
	int diff = !emu->IsZero() ? emu->GetSignCode32(1) : 0;
	emu->EIP += (diff + 5);
}

void jnl_rel32(Emulator *emu){
	int diff = emu->IsSign() == emu->IsOverflow() ? emu->GetSignCode32(1) : 0;
	emu->EIP += (diff + 5);
}

void jl_rel32(Emulator *emu){
	int diff = emu->IsSign() != emu->IsOverflow() ? emu->GetSignCode32(1) : 0;
	emu->EIP += (diff + 5);
}

void setnz_rm8(Emulator *emu){
    emu->EIP++;
    ModRM *modrm = new ModRM(emu);
	emu->reg[emu->instr.RM].reg32 = !emu->IsZero();
    emu->EIP += 2;
}

void sub_rm8_r8(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);
	uint8_t rm8 = modrm->GetRM8();
	uint8_t r8 = modrm->GetR8();
	modrm->SetRM8(rm8-r8);
	emu->update_eflags_sub(rm8, r8);
}

void cmp_rm8_r8(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);
	uint8_t	rm8 = modrm->GetRM8();
	uint8_t r8 = modrm->GetR8();
	emu->update_eflags_sub(rm8, r8);
}

void cmp_rm32_r32(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);
	uint32_t rm32 = modrm->GetRM32();
	uint32_t r32 = modrm->GetR32();
	emu->update_eflags_sub(rm32, r32);
}

void cmp_r32_rm32(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);
	uint32_t r32 = modrm->GetR32();
	uint32_t rm32= modrm->GetRM32();
	emu->update_eflags_sub(r32, rm32);
}

void cmp_eax_imm32(Emulator *emu){
	uint32_t eax = emu->reg[0].reg32;
	uint32_t imm32 = emu->GetCode32(1);
	emu->update_eflags_sub(eax, imm32);
	emu->EIP += 5;
}

void jl(Emulator *emu){
	int diff = (emu->IsSign() != emu->IsOverflow()) ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}

void jle(Emulator *emu){
	int diff = (emu->IsZero() || (emu->IsSign() != emu->IsOverflow())) ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}

void popfd(Emulator *emu){
	emu->set_eflags(emu->Pop32());
	emu->EIP++;
}

void test_eax_imm32(Emulator *emu){
	emu->EIP++;
	emu->update_eflags_and(emu->EAX, emu->GetSignCode32(0));
	emu->EIP += 4;

}

void and_eax_imm32(Emulator *emu){
	emu->EIP++;
	uint32_t imm32 = emu->GetSignCode32(0);
	uint32_t eax = emu->EAX;
	emu->EAX = eax & imm32;
	emu->update_eflags_and(eax,imm32);
	emu->EIP += 4;
}

void test_rm8_r8(Emulator *emu){
    emu->EIP++;
	ModRM modrm(emu);
	uint32_t rm8 = modrm.GetRM8();
	uint32_t r8 = modrm.GetR8();
	emu->update_eflags_and(rm8, r8);
}

void test_rm32_r32(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t rm32 = modrm.GetRM32();
	uint32_t r32 = modrm.GetR32();
	emu->update_eflags_and(rm32, r32);
}

void cli(Emulator *emu){
	emu->set_interrupt(false);
	emu->EIP++;
}

void imul_r32_rm32_imm8(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	int32_t rm32_s = modrm.GetRM32();
	int32_t imm8 = emu->GetSignCode8(0);
	modrm.SetR32(rm32_s * imm8);
	emu->update_eflags_imul(rm32_s, imm8);
	emu->EIP++;
}

void movsx_r32_rm16(Emulator *emu){
    emu->EIP++;
    ModRM modrm(emu);
	int16_t	rm16_s = modrm.GetRM16();
	modrm.SetR32(rm16_s);
}

void mov_eax_moffs32(Emulator *emu){
	emu->EIP++;
	emu->EAX = emu->GetMemory32(emu->GetSignCode32(0));
	emu->EIP += 4;
}
void imul_r32_rm32_imm32(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	int32_t rm32_s = modrm.GetRM32();
	int32_t imm32 = emu->GetSignCode32(0);
	modrm.SetR32(rm32_s * imm32);
	emu->update_eflags_imul(rm32_s, imm32);
	emu->EIP += 4;
}

void imul_r32_rm32(Emulator *emu){
    emu->EIP++;
    ModRM modrm(emu);
   	int16_t r32_s = modrm.GetR32();
   	int16_t rm32_s = modrm.GetRM32();
   	modrm.SetR32(r32_s * rm32_s);
   	emu->update_eflags_imul((int32_t)r32_s, (int32_t)rm32_s);
}
void mov_sreg_rm16(Emulator *emu){
    emu->EIP++;
    ModRM modrm(emu);
    uint16_t rm16 = modrm.GetRM16();
    emu->sreg[emu->instr.reg_index].sreg = rm16;
}

void movzs_r32_rm16(Emulator *emu){
    emu->EIP++;
    ModRM modrm(emu);
    uint16_t rm16 = modrm.GetRM16();
    modrm.SetR32(rm16);
}
} // namespace instruction32

void cdq(Emulator *emu){
	uint32_t eax = emu->EAX;
	emu->EDX = eax&(1<<31) ? -1 : 0;
	emu->EIP++;
}

void cmp_r8_rm8(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	uint8_t r8 = modrm.GetR8();
	uint8_t rm8 = modrm.GetRM8();
	emu->update_eflags_sub(r8, rm8);
}



using namespace instruction32;

void InitInstructions32(void){
	int i;
	instruction_func_t** func = instructions32;
	func[0x01]	= add_rm32_r32;
	func[0x03]	= add_r32_rm32;
//	func[0x04]	= add_al_imm8;
	func[0x05]	= add_eax_imm32;

	func[0x06]	= push_es;
	func[0x07]	= pop_es;

	func[0x09]  = or_rm32_r32;
	func[0x0b]  = or_rm32_r32;
	func[0x0d]  = or_eax_imm32;

	func[0x16]  = push_ss;
	func[0x17]  = pop_ss;
	func[0x1e]  = push_ds;
	func[0x1f]  = pop_ds;

	func[0x25]  = and_eax_imm32;

	func[0x28]  = sub_rm8_r8;
	func[0x29]  = sub_rm32_r32;
	func[0x2b]  = sub_r32_rm32;
	func[0x2d]  = sub_eax_imm32;

	//func[0x3C]	= cmp_al_imm8;
	//func[0x3D]	= cmp_eax_imm32;
	func[0x31]	= xor_rm32_r32;
	func[0x33]	= xor_r32_rm32;
	func[0x35]	= xor_eax_imm32;

	func[0x38]	= cmp_rm8_r8;
	func[0x39]  = cmp_rm32_r32;
	func[0x3a]  = cmp_r8_rm8;
	func[0x3b]  = cmp_r32_rm32;
	func[0x3c]  = cmp_al_imm8;
	func[0x3d]  = cmp_eax_imm32;

	for(i=0;i<8;i++){
		func[0x40 + i]	= inc_r32;
	}
	for(i=0;i<8;i++){
		func[0x48 + i]	= dec_r32;
	}



	for(i=0;i<8;i++){
		func[0x50 + i]	= push_r32;
	}



	for(i=0;i<8;i++){
		func[0x58 + i]	= pop_r32;
	}


	func[0x60]	= pushad;
	func[0x61]	= popad;

	func[0x69]	= imul_r32_rm32_imm32;

	func[0x68]	= push_imm32;
	func[0x6A]	= push_imm8;

	func[0x6B]	= imul_r32_rm32_imm8;

	func[0x70]	= jo;
	func[0x71]	= jno;
	func[0x72]	= jc;
	func[0x73]	= jnc;
	func[0x74]	= jz_rel8;
	func[0x75]	= jnz;
	func[0x76]	= jbe;
	func[0x77]	= ja;
	func[0x78]	= js;
	func[0x79]	= jns;
	func[0x7C]	= jl;
	func[0x7D]	= jnl;
	func[0x7E]	= jle;
	func[0x7F]	= jnle;

	func[0x80]	= code_80;
    func[0x81]	= code_81;
	func[0x83]	= code_83;

	func[0x84]	= test_rm8_r8;
	func[0x85]	= test_rm32_r32;

	func[0x88]	= mov_rm8_r8;
	func[0x89]	= mov_rm32_r32;
	func[0x8A]	= mov_r8_rm8;
	func[0x8B]	= mov_r32_rm32;

	func[0x8D]	= lea_r32_m32;
    func[0x8E]	= mov_sreg_rm16;

	func[0x90]	= nop;
	func[0x99]	= cdq;
	func[0x9C]	= pushfd;
	func[0x9D]	= popfd;

	func[0xA0]	= mov_al_moffs8;
	func[0xA1]	= mov_eax_moffs32;
	func[0xA3]	= mov_moffs32_eax;

	func[0xA9]	= test_eax_imm32;

	for(i=0;i<8;i++){
		func[0xB0 + i]	= mov_r8_imm8;
	}

	for(i=0;i<8;i++){
		func[0xB8 + i]	= mov_r32_imm32;
	}
	func[0xC0]	= code_c0;
	func[0xC1]	= code_c1;

	func[0xC3]	= ret;
	func[0xC6]	= mov_rm8_imm8;
	func[0xC7]	= mov_rm32_imm32;
	func[0xC9]	= leave;

	func[0xd3]	= code_d3;
	//func[0xCD]	= swi;

	func[0xE8]	= call_rel32;
	func[0xE9]	= near_jump;
	func[0xEB]	= short_jump;

	func[0xEC]	= in_al_dx;
	func[0xEE]	= out_dx_al;


	func[0xF6]	= code_f6;
	func[0xF7]	= code_f7;
	func[0xFA]	= cli;
	func[0xFB]	= sti;
	func[0xFF]	= code_ff;

	func[0x0f00]	= code_0f00;
	func[0x0f01]	= code_0f01;
	func[0x0f84]	= jz_rel32;
	func[0x0f85]	= jnz_rel32;
	func[0x0f88]	= js_rel32;
	func[0x0f8d]	= jnl_rel32;
	func[0x0f8e]	= jle_rel32;
	func[0x0f8f]	= jnle_rel32;
	func[0x0f8c]	= jl_rel32;
	func[0x0f95]	= setnz_rm8;
	func[0x0fb6]	= movzs_r32_rm8;
    func[0x0fb7]	= movzs_r32_rm16;
	func[0x0fbf]	= movsx_r32_rm16;

	func[0x0fAF]	= imul_r32_rm32;
	func[0x0fBE]	= movsx_r32_rm8;

}


