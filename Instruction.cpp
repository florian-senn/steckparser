#include "Instruction.h"
#include <stdexcept>

using namespace StackAssembly;

Instruction::Instruction(INSTRUCTIONS inst) : m_inst(inst), m_arg(std::nullopt) {
    switch (inst) {
        case NOP:
        case ADD:
        case SUB:
        case MUL:
        case MOD:
        case IN:
        case OUT:
        case HALT:
            break;
        default:
            throw std::runtime_error("Class Instruction: Invalid instruction: No argument allowed");
    }
}

Instruction::Instruction(INSTRUCTIONS inst, const short arg) : m_inst(inst), m_arg(std::make_optional(arg)) {
    switch (inst) {
        case LDI:
        case LDS:
        case STS:
        case JUMP:
        case JE:
        case JNE:
        case JLT:
        case CALL:
        case RETURN:
        case ALLOC:
            break;
        default:
            throw std::runtime_error("Class Instruction: Invalid instruction: Missing argument");
    }
}

INSTRUCTIONS Instruction::getInstruction() const {
    return m_inst;
}

short Instruction::getArgument() const {
    return m_arg.value();
}