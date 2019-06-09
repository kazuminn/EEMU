#include <iostream>
using namespace std;

#include "Emulator.h"
#include "ModRM.h"

instruction_func_t* instructions32[0xffff];

namespace instruction32{

// 圧倒的NOP
void nop(Emulator *emu){
	emu->EIP++;
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

	emu->set_gdtr(base, limit);
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
    modrm.SetRM32(rm32 + r32);
	emu->update_eflags_add(r32, rm32);
}

void add_eax_imm32(Emulator *emu){
    uint32_t imm32 = emu->GetSignCode32(1);
    uint32_t eax = emu->reg[0].reg32;
	emu->reg[0].reg32 = emu->reg[0].reg32 + imm32;
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
	uint64_t result = (uint64_t)r32 - (uint64_t)1; //32bit目を観測したいから64bitとして扱う
	emu->update_eflags_sub(r32, 1, result);
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
	uint32_t val	= emu->GetCode8(1);
	emu->reg[reg].reg32 = val;
	emu->EIP += 5;
}

void mov_r8_rm8(Emulator *emu){//		cout<<"mov_r8_rm8"<<endl;
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t rm8 = modrm.GetRM8();
	modrm.SetR8(rm8);
}

void mov_r32_rm32(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t rm32 = modrm.GetRM32();
	printf("rm32 : %x\n", rm32);
	printf("EBX : %x\n", emu->EBX);
	printf("GetRegister32 : %x\n", emu->GetRegister32(emu->instr.RM));
	printf("disp8 : %x\n", emu->instr.disp8);
	for(int i = 0;i<40;i++){
		printf("ESP + i: %x\n", emu->GetMemory32(emu->ESP + i));
	}

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
	emu->EIP += 4;
}


void add_rm32_imm8(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint32_t imm8 = (int32_t)emu->GetSignCode8(0);
	emu->EIP++;
	modrm->SetRM32(rm32 + imm8);
	emu->update_eflags_add(rm32, imm8);
}

void mov_rm32_imm32(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t val = emu->GetCode32(0);
	emu->EIP+=4;
	modrm.SetRM32(val);		cout<<"mov val="<<val<<endl;
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
     uint8_t imm8 = emu->GetSignCode8(1);
	 emu->EIP += imm8 + 2;
}

void near_jump(Emulator *emu){
	int32_t diff = emu->GetSignCode32(1);
	emu->EIP += (diff + 5);
}

void sub_rm32_imm32(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint32_t imm32 = (int32_t)emu->GetSignCode32(0);
	emu->EIP += 4;
	modrm->SetRM32(rm32 - imm32);
	uint64_t result = (uint64_t)rm32 - (uint64_t)imm32; //32bit目を観測したいから64bitとして扱う
	emu->update_eflags_sub(rm32, imm32, result);
}
void push_imm32(Emulator *emu) {
	uint32_t imm32 = (int32_t)emu->GetSignCode32(0);
	emu->Push32(imm32);
	emu->EIP += 5;
}

void sub_rm32_imm8(Emulator *emu, ModRM *modrm){
    uint32_t rm32 = modrm->GetRM8();
    uint32_t imm8 = (int32_t)emu->GetSignCode8(0);
    emu->EIP++;
    modrm->SetRM32(rm32 - imm8);
	uint64_t result = (uint64_t)rm32 - (uint64_t)imm8; //32bit目を観測したいから64bitとして扱う
	emu->update_eflags_sub(rm32, imm8, result);
}


void sub_rm32_r32(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t rm32 = modrm.GetRM8();
	uint32_t r32 = modrm.GetR32();
	modrm.SetRM32(rm32 - r32);
	uint64_t result = (uint64_t)rm32 - (uint64_t)r32; //32bit目を観測したいから64bitとして扱う
	emu->update_eflags_sub(rm32, r32, result);
}

void sub_r32_rm32(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t r32 = modrm.GetR32();
	uint32_t rm32 = modrm.GetRM8();
	modrm.SetR32(rm32 - r32);
	uint64_t result = (uint64_t)r32 - (uint64_t)rm32; //32bit目を観測したいから64bitとして扱う
	emu->update_eflags_sub(r32, rm32, result);
}

void sub_eax_imm32(Emulator *emu) {
    uint32_t imm32 = emu->GetSignCode32(1); uint32_t eax = emu->reg[0].reg32;
	emu->reg[0].reg32 = emu->reg[0].reg32 ^ imm32;
	emu->EIP += 5;
	uint64_t result = (uint64_t)eax - (uint64_t)imm32; //32bit目を観測したいから64bitとして扱う
	emu->update_eflags_sub(eax , imm32, result);
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
	uint8_t imm8 = emu->GetSignCode8(0);
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
	uint8_t imm8 = emu->GetSignCode8(0);
	emu->EIP += 4;
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
	uint64_t result = (uint64_t)rm32 - (uint64_t)imm32; //32bit目を観測したいから64bitとして扱う
	emu->update_eflags_sub(rm32, imm32, result);
	emu->EIP += 4;
}

void cmp_rm32_imm8(Emulator *emu, ModRM *modrm){
	uint32_t rm32= modrm->GetRM32();
	uint32_t imm8 = emu->GetCode8(0);
	uint64_t result = (uint64_t)rm32 - (uint64_t)imm8; //32bit目を観測したいから64bitとして扱う
	emu->update_eflags_sub(rm32, imm8, result);
	emu->EIP += 4;
}

void shl_rm32_imm8(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint32_t imm8 = emu->GetCode8(0);
	modrm->SetRM32(rm32<<imm8);
	emu->update_eflags_shl(rm32, imm8);
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
	uint32_t rm32 = modrm->GetRM32();
	uint32_t imm8 = emu->GetCode8(0);
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
	int8_t rm8_s = modrm->GetR32();
	uint8_t imm8 = emu->GetSignCode8(0);
	modrm->SetRM8(rm8_s>>imm8);
	emu->EIP += 1;
}
void and_rm32_imm8(Emulator *emu, ModRM *modrm) {
	uint32_t rm32 = modrm->GetRM32();
	uint8_t imm8 = emu->GetSignCode8(0);
	modrm->SetRM32(rm32&imm8);
	emu->EIP++;
}

void code_81(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);

	switch(emu->instr.opecode){
		case 0: add_rm32_imm32(emu, modrm);  break;
		case 1: or_rm32_imm32(emu, modrm);   break;
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
		case 7: sar_rm8_imm8(emu, modrm); break;
		default:
			cout<<"not implemented: c1 "<<(uint32_t)emu->instr.opecode<<endl;
	}
	delete modrm;
}

void code_c1(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);

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
	uint8_t reg = emu->GetCode8(0) - 0x50;
	emu->Push32(emu->GetRegister32(reg));
	emu->EIP++;
}

void pop_r32(Emulator *emu){
	uint8_t reg = emu->GetCode8(0) - 0x58;
	emu->SetRegister32(reg, emu->Pop32());
	emu->EIP++;
}

void push_imm8(Emulator *emu){
	uint32_t val = emu->GetCode32(1);
	emu->Push32(val);
	emu->EIP += 2;
}

void inc_rm32(Emulator *emu, ModRM *modrm){
	uint32_t val = modrm->GetRM32();
	modrm->SetRM32(val + 1);
}

void code_ff(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);
	
	switch(emu->instr.opecode){
	case 0:
		inc_rm32(emu, modrm);
		break;
	default:
		cout<<"not implemented: 0xFF /"<<(int)emu->instr.opecode<<endl;
	}
	
	delete modrm;
}

void call_rel32(Emulator *emu){
	int32_t diff = emu->GetSignCode32(1);
	emu->Push32(emu->EIP + 5);	//call命令は全体で5バイト
	emu->EIP += (diff + 5);
	for(int i = 0;i<3;i++){
		printf("ESP + i: %x\n", emu->GetMemory32(emu->ESP + i));
	}
}

void ret(Emulator *emu){//	cout<<"ret"<<endl;
	if(emu->EIP == 0x172a){
		for(int i = 0;i<40;i++){
			printf("ESP + i: %x\n", emu->GetMemory32(emu->ESP + i));
		}
	}
	emu->EIP = emu->Pop32();
	printf("ESP - 4 %x\n", emu->GetMemory32(emu->ESP - 4));
	printf("EIP - 4 %x\n", emu->GetMemory32(emu->EIP - 6));
	printf("ret: %x\n", emu->memory[emu->sgregs[1].base + emu->EIP + 10]);
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
void jz(Emulator *emu){
	int diff = emu->IsZero() ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
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


#undef DEFINE_JX
void cmp_rm32_r32(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);
	uint32_t rm32= modrm->GetRM32();
	uint32_t r32 = modrm->GetR32();
	uint64_t result = (uint64_t)rm32 - (uint64_t)r32; //32bit目を観測したいから64bitとして扱う
	emu->update_eflags_sub(rm32, r32, result);
	}

void cmp_r32_rm32(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);
	uint32_t r32 = modrm->GetR32();
	uint32_t rm32= modrm->GetRM32();
	uint64_t result = (uint64_t)r32 - (uint64_t)rm32; //32bit目を観測したいから64bitとして扱う
	emu->update_eflags_sub(r32, rm32, result);
}

void cmp_eax_imm32(Emulator *emu){
	uint32_t eax = emu->reg[0].reg32;
	uint32_t imm32 = emu->GetCode32(1);
	uint64_t result = (uint64_t)eax - (uint64_t)imm32; //32bit目を観測したいから64bitとして扱う
	emu->update_eflags_sub(eax, imm32, result);
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

} // namespace instruction32



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

	func[0x29]  = sub_rm32_r32;
	func[0x2b]  = sub_r32_rm32;
	func[0x2d]  = sub_eax_imm32;

	//func[0x3C]	= cmp_al_imm8;
	//func[0x3D]	= cmp_eax_imm32;
	func[0x31]	= xor_rm32_r32;
	func[0x33]	= xor_r32_rm32;
	func[0x35]	= xor_eax_imm32;

	func[0x39]  = cmp_rm32_r32;
	func[0x3b]  = cmp_r32_rm32;
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

	func[0x68]	= push_imm32;
	func[0x6A]	= push_imm8;
	
	func[0x70]	= jo;
	func[0x71]	= jno;
	func[0x72]	= jc;
	func[0x73]	= jnc;
	func[0x74]	= jz;
	func[0x75]	= jnz;
	func[0x76]	= jbe;
	func[0x78]	= js;
	func[0x79]	= jns;
	func[0x7C]	= jl;
	func[0x7E]	= jle;

    func[0x81]	= code_81;
	func[0x83]	= code_83;
	func[0x88]	= mov_rm8_r8;
	func[0x89]	= mov_rm32_r32;
	//func[0x8A]	= mov_r8_rm8;
	func[0x8B]	= mov_r32_rm32;

	func[0x8D]	= lea_r32_m32;

	func[0x90]	= nop;

	func[0xA0]	= mov_al_moffs8;

	for(i=0;i<8;i++){
		func[0xB0 + i]	= mov_r8_imm8;
	}
	
	for(i=0;i<8;i++){
		func[0xB8 + i]	= mov_r32_imm32;
	}
	func[0xC0]	= code_c0;
	func[0xC1]	= code_c1;

	func[0xC3]	= ret;
	func[0xC7]	= mov_rm32_imm32;
	func[0xC9]	= leave;

	func[0xd3]	= code_d3;
	//func[0xCD]	= swi;
	
	func[0xE8]	= call_rel32;
	func[0xE9]	= near_jump;
	func[0xEB]	= short_jump;
	//func[0xEC]	= in_al_dx;
	//func[0xEE]	= out_dx_al;
	func[0xFF]	= code_ff;

	func[0x0f01]	= code_0f01;
}


