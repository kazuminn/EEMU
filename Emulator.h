#ifndef EMULATOR_H_
#define EMULATOR_H_

#include <stdio.h>
#include <stdint.h>

#define DEFAULT_BIT_MODE	32//16		//デフォルトの起動時のビット。本来は16。
#define DEFAULT_MEMORY_SIZE	(1024 * 1024)	//デフォルトのメモリサイズ。1MB
#define REGISTERS_COUNT		8		//レジスタの本数(16/32bit)

#define VRAM_ADDR		0xa0000

#define LOW			0
#define HIGH			1

extern const char* registers_name16[];		//16bitレジスタの名前
extern const char* registers_name32[];		//32bitレジスタの名前


#define REGISTERS_COUNT32 8

#define  AL reg[0].low8
#define  CL reg[1].low8
#define  DL reg[2].low8
#define  BL reg[3].low8
#define  AH reg[0].high8
#define  CH reg[1].high8
#define  DH reg[2].high8
#define  BH reg[3].high8

//16bitレジスタに簡単にアクセスするためのdefine
#define  AX	reg[0].reg16
#define  CX	reg[1].reg16
#define  DX	reg[2].reg16
#define  BX	reg[3].reg16
#define  SP	reg[4].reg16
#define  BP	reg[5].reg16
#define  SI	reg[6].reg16
#define  DI	reg[7].reg16

//32bitレジスタに簡単にアクセスするためのdefine
#define EAX	reg[0].reg32
#define ECX	reg[1].reg32
#define EDX	reg[2].reg32
#define EBX	reg[3].reg32
#define ESP	reg[4].reg32
#define EBP	reg[5].reg32
#define ESI	reg[6].reg32
#define EDI	reg[7].reg32

#define ES	sreg[0].sreg;
#define CS	sreg[1].sreg;
#define SS	sreg[2].sreg;
#define DS	sreg[3].sreg;
#define FS	sreg[4].sreg;
#define GS	sreg[5].sreg;

//特殊なレジスタに簡単にアクセスするためのdefine
#define FLAGS	eflags.reg16
#define EFLAGS	eflags.reg32
#define IP	eip.reg16
#define EIP	eip.reg32

//GDT,IDT関連
//GDTR,IDTR用のレジスタ型(48bit)
struct DTRegister {
	uint16_t table_limit;
	uint32_t base_addr;
};

//GDT
struct SEGMENT_DESCRIPTOR {
	uint16_t limit_low, base_low;
	uint8_t base_mid, access_right;
	uint8_t limit_high, base_high;
};

//IDT
struct GATE_DESCRIPTOR {
	uint16_t offset_low, selector;
	uint8_t dw_count, access_right;
	uint16_t offset_high;
};


// EFLAGS
class Register {
public:
	struct {
		uint32_t reg32;
		uint16_t reg16;
		uint16_t sreg;
		struct {
			uint8_t low8;
			uint8_t high8;
		};
		struct {
			bool CF : 1;
			bool    : 1;
			bool PF : 1;
			bool    : 1;
			bool AF : 1;
			bool    : 1;
			bool ZF : 1;
			bool SF : 1;
			bool TF : 1;
			bool IF : 1;
			bool DF : 1;
			bool OF : 1;
			bool IOPL1 : 1;
			bool IOPL2 : 1;
			bool NT : 1;
			bool    : 1;
			bool RF : 1;
			bool VM : 1;
			bool AC : 1;
			bool VIF: 1;
			bool VIP: 1;
			bool ID : 1;
		};
	};
};

//エミュレータクラス
class Emulator{
private:
	int BitMode;
	int memory_size;
	Register eflags;
	Register sreg[6];
public:
	Register CR[5];		// CR0 ~ CR4 制御レジスタ

	Register eip;
	DTRegister GDTR, IDTR;
	
	Register reg[REGISTERS_COUNT];
	
	uint8_t *memory;
public:				// member funcs
	Emulator();
	~Emulator();

	void InitRegisters();

	int GetBitMode();
	bool IsReal(){	return !IsProtected();	}
	bool IsProtected();
	size_t GetMemSize(){	return memory_size;	}
	
	void LoadBinary(const char* fname, uint32_t addr, int size);	//バイナリファイルを読み込んでメモリの指定番地に指定サイズだけ転送する
	
	uint8_t GetCode8(int index);
	int8_t  GetSignCode8(int index);
	uint32_t GetCode32(int index);
	int32_t GetSignCode32(int index);

	uint8_t GetRegister8(int index);
    uint16_t GetRegister16(int index);
	uint32_t GetRegister32(int index);

	void SetRegister8(int index, uint8_t val);
	void SetRegister16(int index, uint16_t val);
	void SetRegister32(int index, uint32_t val);

	uint32_t GetMemory8(uint32_t addr);
	uint32_t GetMemory16(uint32_t addr);
	uint32_t GetMemory32(uint32_t addr);
	void SetMemory8(uint32_t addr, uint32_t val);
	void SetMemory16(uint32_t addr, uint32_t val);
	void SetMemory32(uint32_t addr, uint32_t val);

	void Push32(uint32_t val);
	uint32_t Pop32();


	void DumpRegisters(int bit);		//各レジスタの値を標準入出力に書き込む。引数はビットモード。
	void DumpRegisters();
	void DumpMemory(const char *fname, uint32_t addr, uint32_t size);
	void DumpMemory(const char *fname, uint32_t size){	DumpMemory(fname, 0x00, size);	}
	void DumpMemory(const char *fname){	DumpMemory(fname, memory_size);	}
public:
	inline int IsAjust() { return eflags.AF; }
	inline int IsCarry() { return eflags.CF; }
	inline int IsParity() { return eflags.PF; }
	inline int IsZero() { return eflags.ZF; }
	inline int IsSign() { return eflags.SF; }
	inline int IsOverflow() { return eflags.OF; }
	inline int IsInterrupt() { return eflags.IF; }
	inline int IsDirection() { return eflags.DF; }
	inline void SetAjust(bool ajust) { eflags.AF = ajust; }
	inline void SetCarry(bool carry) { eflags.CF = carry; }
	inline void SetParity(bool parity) { eflags.PF = parity; }
	inline void SetZero(bool zero) { eflags.ZF = zero; }
	inline void SetSign(bool sign) { eflags.SF = sign; }
	inline void SetOverflow(bool of) { eflags.OF = of; }
	inline void SetInterrupt(bool intr) { eflags.IF = intr; }
	inline void SetDirection(bool dir) { eflags.DF = dir; }

	inline void UpdateXor(){
		eflags.CF = eflags.OF = 0;
	}

	inline void UpdateOr(){
		eflags.CF = eflags.OF = 0;
	}

	template <class T> uint32_t update_eflags_add(T v1, uint32_t v2);
    template <class T> uint32_t update_eflags_shl(T v, uint8_t c);
	template <class T> uint32_t update_eflags_shr(T v, uint8_t c);
	void update_eflags_sub(uint32_t v1, uint32_t v2, uint64_t result);

	void set_ES(uint16_t v) { sreg[0].sreg = v;}
	void set_CS(uint16_t v) { sreg[1].sreg = v;}
	void set_SS(uint16_t v) { sreg[2].sreg = v;}
	void set_DS(uint16_t v) { sreg[3].sreg = v;}
	void set_FS(uint16_t v) { sreg[4].sreg = v;}
	void set_GS(uint16_t v) { sreg[5].sreg = v;}

	uint16_t get_ES() {return ES;}
	uint16_t get_CS() {return CS;}
	uint16_t get_SS() {return SS;}
	uint16_t get_DS() {return DS;}
	uint16_t get_FS() {return FS;}

	void set_AL(uint8_t v) { reg[0].low8 = v;}
	void set_CL(uint8_t v) { reg[1].low8 = v;}
	void set_DL(uint8_t v) { reg[2].low8 = v;}
	void set_BL(uint8_t v) { reg[3].low8 = v;}
	void set_AH(uint8_t v) { reg[0].high8 = v;}
	void set_CH(uint8_t v) { reg[1].high8 = v;}
	void set_DH(uint8_t v) { reg[2].high8 = v;}
	void set_BH(uint8_t v) { reg[3].high8 = v;}

	uint8_t get_AL() {return AL;}
	uint8_t get_CL() {return CL;}
	uint8_t get_DL() {return DL;}
	uint8_t get_BL() {return BL;}
	uint8_t get_AH() {return AH;}
	uint8_t get_CH() {return CH;}
	uint8_t get_DH() {return DH;}
	uint8_t get_BH() {return BH;}
private:
	bool chk_parity(uint8_t v);
};

//instructions

typedef void instruction_func_t(Emulator*);	//各命令に対応した関数の型

void InitInstructions16(void);			//16bit命令の初期化
void InitInstructions32(void);			//32bit

extern instruction_func_t* instructions16[256];	//16bit命令の関数の配列
extern instruction_func_t* instructions32[256];	//32bit

//eflgas

#endif //EMULATOR_H_
