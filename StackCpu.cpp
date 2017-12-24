//
// Created by Admin on 12/12/2017.
//

#include "StackCpu.h"
#include <iostream>
#include <stack>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

using namespace StCpu;

StackCpu::StackCpu(const std::vector<StackAssembly::Instruction> &t_program) : m_program(t_program) {
    m_stack.fill(-1);
}

void StackCpu::push(const int t_value) {
    m_stackHeight++;
    m_stack.at(m_stackHeight) = t_value;
}

int StackCpu::pop() {
    int value = m_stack.at(m_stackHeight);
    m_stack.at(m_stackHeight) = -1;
    m_stackHeight--;

    return value;
}

int StackCpu::execute() {
    for (std::size_t i = 0; i < m_program.size(); i++) {
        using namespace StackAssembly;

        const auto &program = m_program.at(i);

        switch (program.getInstruction()) {
            case NOP:
                break;
            case ADD:
                add();
                break;
            case SUB:
                sub();
                break;
            case MUL:
                mul();
                break;
            case MOD:
                mod();
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
                i = static_cast<std::size_t>(program.getArgument() - 1);
                break;
            case JE:
                if (!jumpIfFalse()) {
                    i = static_cast<std::size_t>(program.getArgument() - 1);
                }
                break;
            case JNE:
                if (jumpIfFalse()) {
                    i = static_cast<std::size_t>(program.getArgument() - 1);
                }
                break;
            case JLT:
                if(jumpIfSmaller()) {
                    i = static_cast<std::size_t>(program.getArgument() -1);
                }
                break;
            case CALL:
                i = callFunction(program.getArgument(), i);
                break;
            case RETURN:
                i = returnFromFunction(program.getArgument());
                break;
            case IN:
                readValue();
                break;
            case OUT:
                writeValue();
                break;
            case HALT:
                return pop();
            case ALLOC:
                allocate(program.getArgument());
                break;
            default:
                throw std::runtime_error("Class StackCpu: Unimplemented instruction");
        }
    }
    return 0;
}

void StackCpu::add() {
    push(pop() + pop());
}

void StackCpu::sub() {
    push(pop() - pop());
}

void StackCpu::mul() {
    push(pop() * pop());
}

void StackCpu::mod() {
    push(pop() % pop());
}

void StackCpu::loadInteger(const short t_value) {
    push(t_value);
}

void StackCpu::localToStack(const short t_offset) {
    push(m_stack.at(m_fPointer + t_offset));
}

void StackCpu::stackToLocal(const short t_offset) {
    m_stack.at(m_fPointer + t_offset) = pop();
}

bool StackCpu::jumpIfFalse() {
    auto first_arg = pop();
    auto second_arg = pop();

    return first_arg != second_arg;
}

bool StackCpu::jumpIfSmaller() {
    auto first_arg = pop();
    auto second_arg = pop();

    return first_arg < second_arg;
}

std::size_t StackCpu::callFunction(const short t_amount, const std::size_t t_callback) {
    //Save address location of function
    short address = static_cast<short>(pop() - 1);
    //Consume all arguments
    std::stack<int> args;
    for (short j = 0; j < t_amount; j++) {
        args.push(pop());
    }
    //Set previous framePointer location
    push(m_fPointer);
    //Save address location of previous function
    push(static_cast<int>(t_callback));
    //Put all args on top of the stack
    while(!args.empty()) {
        push(args.top());
        args.pop();
    }
    //Set frame pointer to new location
    m_fPointer = m_stackHeight;
    return static_cast<std::size_t>(address);
}

std::size_t StackCpu::returnFromFunction(const int t_amount) {
    //Save return value
    auto return_value = static_cast<short>(pop());
    //Remove locale variables and function arguments
    m_stackHeight -= t_amount;
    //Save callback function pointer
    std::size_t callback = static_cast<std::size_t>(pop());
    //Reset frame pointer to original location
    m_fPointer = static_cast<unsigned short>(pop());
    //Save return value on top of stack
    push(return_value);

    return callback;
}

void StackCpu::readValue() {
    std::string input;
    std::cout << "Please enter a value of type short: ";
    std::getline(std::cin, input);
    short temp;
    try {
        temp = static_cast<short>(std::stoi(input));
    } catch (const std::invalid_argument &e) {
        throw std::runtime_error("Illegal input " + input + ": " + e.what());
    }
    push(temp);
}

void StackCpu::writeValue() {
    std::cout << "Consumed value of stack: " << pop() << std::endl;
}

void StackCpu::allocate(const short t_amount) {
    (void)t_amount;
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
    for (std::size_t i = 0; i < lines.size(); i++) {
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

std::vector<std::string> StackCpu::splitString(const std::string &t_str, const char t_split_char) {
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
