#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <optional>

namespace StackAssembly {
    enum INSTRUCTIONS {
        NOP,
        ADD,
        SUB,
        MUL,
        MOD,
        LDI,
        LDS,
        STS,
        JUMP,
        JE,
        JNE,
        JLT,
        IN,
        OUT,
        CALL,
        RETURN,
        HALT,
        ALLOC
    };

    class Instruction {
    private:
        enum INSTRUCTIONS m_inst;
        std::optional<short> m_arg;

    public:
        explicit Instruction(INSTRUCTIONS inst);

        explicit Instruction(INSTRUCTIONS inst, short arg);

        INSTRUCTIONS getInstruction() const;

        short getArgument() const;
    };
}
#endif 