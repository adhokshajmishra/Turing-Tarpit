#include <iostream>
#include <cstdint>
#include <functional>
#include <chrono>

#define RAM_SIZE 1000
#define REG_NBR 16

class Ram {
    private:
        uint16_t ram_[RAM_SIZE];
        
    public:
        uint16_t Get(uint16_t address) {
            if (address <= RAM_SIZE) {
                return ram_[address];
            }
            else
                return 0;
        }
        
        uint16_t Set(uint16_t address, uint16_t value) {
            if (address <= RAM_SIZE) {
                ram_[address] = value;
                return 1;
            }
            else
                return 0;
        }
};

class CPU {
    public:
        void Process() {
    	    while (true) {
	            _instr[_addr.Get(_pc)](this);
	            ++_pc;
	        }
	    }
	    
	    template <unsigned char Opcode, class Inst> void RegisterOpcode() {
            _instr[Opcode] = std::function<void(CPU*)>(&Inst::Do);
        }

        uint16_t _regs[REG_NBR], _flags, _sp, _pc;
        Ram _addr;
        std::function<void(CPU*)> _instr[256];
};

enum RegisterLetter {A = 0, B, C, D, E, F };

template <int Reg> class Register {
    public:
        static inline uint16_t Get(CPU* proc) {
            return proc->_regs[Reg];
        }

        static inline void Set(CPU* proc, uint16_t val) {
            proc->_regs[Reg] = val;
        }
};

struct NextByte {
    static uint8_t Get(CPU* p) {
        ++p->_pc;
        return p->_addr.Get(p->_pc);
    }
};

template <template <class, class, class> class Action, class Op1, class Op2, class Op3> class Instr {
    public:
        static void Do(CPU* p) {
            Action<Op1, Op2, Op3>::Do(p);
        }
};

template <class A, class B, class C> struct SUBLEQ {
    static void Do(CPU* p) {
        int res = B::Get(p) - A::Get(p);
        A::Set(p, res);
        
        if (res <= 0) {
            uint16_t new_addr = C::Get(p);
            p->_pc = new_addr;
        }
    }
};

int main() {
    CPU cpu;
    cpu.RegisterOpcode<0xAA, Instr<SUBLEQ, Register<A>, Register<B>, NextByte>>();
	return 0;
}

