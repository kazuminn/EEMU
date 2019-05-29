#ifndef MODRM_H_
#define MODRM_H_

#include "Emulator.h"

struct SIB {
	uint8_t base :3;
	uint8_t index :3;
	uint8_t scale :2;
};

class ModRM {
private:
	Emulator *emu;
public:
	uint8_t Mod;
	union {
		uint8_t opecode;
		uint8_t reg_index;
	};
	uint8_t RM;
	uint8_t SIB;
	int SEGMENT;
	union {
		int8_t disp8;
		uint32_t disp32;
	};
	struct SIB sib;
public:
	ModRM();
	ModRM(Emulator *emu);
//	~ModRM();

	void Parse(Emulator *emu);
	void Parse();

	uint8_t GetRM8(Emulator *emu);
	uint8_t GetRM8();
	void SetRM8(Emulator *emu, uint8_t val);
	void SetRM8(uint8_t val);

	uint32_t calc_modrm32(void);
	uint32_t calc_sib(void);

	uint16_t GetRM16(Emulator *emu);
	uint16_t GetRM16();
	void SetRM16(Emulator *emu, uint16_t val);
	void SetRM16(uint16_t val);

	uint32_t GetRM32(Emulator *emu);
	uint32_t GetRM32();
	void SetRM32(Emulator *emu, uint32_t val);
	void SetRM32(uint32_t val);

	uint8_t GetR8(Emulator *emu);
	uint8_t GetR8();
	void SetR8(Emulator *emu, uint8_t val);
	void SetR8(uint8_t val);

	uint16_t GetR16(Emulator *emu);
	uint16_t GetR16();

	uint32_t GetR32(Emulator *emu);
	uint32_t GetR32();
	void SetR32(Emulator *emu, uint32_t val);
	void SetR32(uint32_t val);

	uint32_t CalcMemAddr(Emulator *emu);
	uint32_t CalcMemAddr();
};

#endif //MODRM_H_

