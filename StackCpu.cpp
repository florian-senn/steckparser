//
// Created by Admin on 12/12/2017.
//

#include "StackCpu.h"
#include <iostream>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

using namespace StackCpuNS;

StackCpu::StackCpu(const std::vector<StackAssembly::Instruction> &t_program) : m_program(t_program) {
    m_stack.fill(-1);
}

void StackCpu::execute() {
    for (int i = 0; i < m_program.size(); i++) {
        using namespace StackAssembly;

        const auto &program = m_program.at(i);
        std::string input;
        short address;
        short return_value;
        switch (program.getInstruction()) {
            case NOP:
                break;
            case ADD:
                add();
                break;
            case SUB:
                sub();
                break;
            case LDI:
                loadInteger(program.getArgument());
                break;
            case LDS:
                localToStack(program.getArgument());
                break;
            case STS:
                stackToLocal(program.getArgument());
                break;
            case JUMP:
                i = program.getArgument() - 1;
                break;
            case JE:
                if (!jumpIfFalse()) {
                    i = program.getArgument() - 1;
                }
                break;
            case JNE:
                if (jumpIfFalse()) {
                    i = program.getArgument() - 1;
                }
                break;
            case CALL:
                //Save address location of function
                address = static_cast<short>(m_stack.at(m_stackHeight--));
                //Set previous framePointer location
                m_stack.at(++m_stackHeight) = m_fPointer;
                //Save address location of previous function
                m_stack.at(++m_stackHeight) = i;
                //Set frame pointer to new location
                m_fPointer = m_stackHeight + 1;
                i = --address;
                break;
            case RETURN:
                return_value = static_cast<short>(m_stack.at(m_stackHeight));
                m_stackHeight -= program.getArgument();
                i = m_stack.at(m_stackHeight--);
                m_fPointer = static_cast<unsigned short>(m_stack.at(m_stackHeight--));
                m_stack.at(++m_stackHeight) = return_value;
                break;
            case IN:
                std::cout << "Please enter a value of type short: ";
                std::getline(std::cin, input);
                short temp;
                try {
                    temp = static_cast<short>(std::stoi(input));
                } catch (const std::invalid_argument &e) {
                    throw std::runtime_error("Illegal input " + input + ": " + e.what());
                }
                m_stack.at(++m_stackHeight) = temp;
                input.clear();
                break;
            case OUT:
                std::cout << m_stack.at(m_stackHeight--) << std::endl;
                break;
            case HALT:
                std::exit(0);
                break;
            case ALLOC:
                allocate(program.getArgument());
                break;
            default:
                throw std::runtime_error("Class StackCpu: Unimplemented instruction");
            //case MUL:break;
            //case MOD:break;
            //case JLT:break;
        }
    }
    std::cout << "Program has successfully finished executing!" << std::endl;
}

void StackCpu::add() {
    m_stack.at(++m_stackHeight) = m_stack.at(m_stackHeight--) + m_stack.at(m_stackHeight--);
}

void StackCpu::sub() {
    m_stack.at(++m_stackHeight) = m_stack.at(m_stackHeight--) - m_stack.at(m_stackHeight--);
}

void StackCpu::loadInteger(const short &t_value) {
    m_stack.at(++m_stackHeight) = t_value;
}

void StackCpu::localToStack(const short &t_offset) {
    m_stack.at(++m_stackHeight) = m_stack.at(m_fPointer + static_cast<unsigned short>(t_offset));
}

void StackCpu::stackToLocal(const short &t_offset) {
    if (t_offset <= 0) {
        throw std::runtime_error("Illegal instruction: Attempting to modify"
                                         "function argument!");
    }

    m_stack.at(m_fPointer + static_cast<unsigned short>(t_offset - 1)) = m_stack.at(m_stackHeight--);
}

bool StackCpu::jumpIfFalse() {
    auto first_arg = m_stack.at(m_stackHeight--);
    auto second_arg = m_stack.at(m_stackHeight--);

    return first_arg != second_arg;
}

void StackCpu::allocate(const short &t_amount) {
//    for(std::size_t i = 0; i < t_amount; i++) {
//        //The m_fPointer is the offset of where the memory
//        //should be allocated.
//        m_stack.at(i + m_fPointer) = 0;
//    }
    ++m_stackHeight;
}


StackCpu StackCpu::parse(const std::string &t_program) {
    std::vector<StackAssembly::Instruction> instructions;
    std::map<std::string, short> jumps;
    std::vector<std::string> lines = splitString(t_program, '\n');

    //Resolve gotos
    for (int i = 0; i < lines.size(); i++) {
        if (boost::ends_with(lines.at(i), ":")) {
            jumps.emplace(lines.at(i).substr(0, lines.at(i).find(':')), i);
            lines.erase(lines.begin() + i);
        }
    }

    for (const auto &line : lines) {
        using namespace StackAssembly;

        Instruction temp(NOP);
        if (boost::starts_with(line, "NOP")) {
            temp = Instruction(NOP);
        } else if (boost::starts_with(line, "ADD")) {
            temp = Instruction(ADD);
        } else if (boost::starts_with(line, "SUB")) {
            temp = Instruction(SUB);
        } else if (boost::starts_with(line, "MUL")) {
            temp = Instruction(MUL);
        } else if (boost::starts_with(line, "MOD")) {
            temp = Instruction(MOD);
        } else if (boost::starts_with(line, "LDI")) {
            temp = Instruction(LDI, resolveArg(line, jumps));
        } else if (boost::starts_with(line, "LDS")) {
            temp = Instruction(LDS, resolveArg(line, jumps));
        } else if (boost::starts_with(line, "STS")) {
            temp = Instruction(STS, resolveArg(line, jumps));
        } else if (boost::starts_with(line, "JUMP")) {
            temp = Instruction(JUMP, resolveArg(line, jumps));
        } else if (boost::starts_with(line, "JE")) {
            temp = Instruction(JE, resolveArg(line, jumps));
        } else if (boost::starts_with(line, "JNE")) {
            temp = Instruction(JNE, resolveArg(line, jumps));
        } else if (boost::starts_with(line, "JLT")) {
            temp = Instruction(JLT, resolveArg(line, jumps));
        } else if (boost::starts_with(line, "CALL")) {
            temp = Instruction(CALL, resolveArg(line, jumps));
        } else if (boost::starts_with(line, "RETURN")) {
            temp = Instruction(RETURN, resolveArg(line, jumps));
        } else if (boost::starts_with(line, "IN")) {
            temp = Instruction(IN);
        } else if (boost::starts_with(line, "OUT")) {
            temp = Instruction(OUT);
        } else if (boost::starts_with(line, "HALT")) {
            temp = Instruction(HALT);
        } else if (boost::starts_with(line, "ALLOC")) {
            temp = Instruction(ALLOC, resolveArg(line, jumps));
        } else {
            throw std::runtime_error("Class StackCpu: Error while parsing line " + line + ": Unknown keyword");
        }
        instructions.push_back(std::move(temp));
    }

    return StackCpu(instructions);
}

std::vector<std::string> StackCpu::splitString(const std::string &t_str, const char &t_split_char) {
    std::vector<std::string> lines;

    std::string::size_type position = 0;
    for (std::size_t i = 0; i < t_str.length(); i++) {
        i = t_str.find(t_split_char, position);

        if (i == std::string::npos) {
            i = t_str.length();
        }

        std::string temp = t_str.substr(position, i - position);
        boost::trim(temp);

        if (temp.empty()) {
            position = i + 1;
            continue;
        } else if (boost::starts_with(temp, "//")) {
            position = i + 1;
            continue;
        }

        lines.push_back(std::move(temp));
        position = i + 1;
    }

    return lines;
}

std::string StackCpu::extractArg(const std::string &t_str) {
    std::string arg;
    try {
        arg = splitString(t_str, ' ').at(1);
    }
    catch (const std::out_of_range &e) {
        throw std::runtime_error("Class StackCpu:: Error while parsing line " + t_str + ": " + e.what());
    }

    return arg;
}

short StackCpu::resolveArg(const std::string &t_str, const std::map<std::string, short> &t_jumps) {
    std::string arg = extractArg(t_str);
    auto search = t_jumps.find(arg);
    if (search != t_jumps.end()) {
        return search->second;
    }

    //Does not exist, try casting it...
    short value;
    try {
        value = static_cast<short>(std::stoi(arg));
    }
    catch (const std::invalid_argument &e) {
        throw std::runtime_error("Class StackCpu: Error while parsing line " + t_str + ": " + e.what());
    }

    return value;
}
