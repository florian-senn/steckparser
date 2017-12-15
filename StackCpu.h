//
// Created by Admin on 12/12/2017.
//

#ifndef STECKASSEMBLER_STACKCPU_H
#define STECKASSEMBLER_STACKCPU_H

#include <array>
#include <vector>
#include <string>
#include <map>

#include "Instruction.h"

namespace StackCpuNS {

    class StackCpu {
    private:
        std::array<int, 128> m_stack;
        std::vector<StackAssembly::Instruction> m_program;
        unsigned short m_stackHeight = 0;
        unsigned short m_fPointer = 0;

        //Constructor
        explicit StackCpu(const std::vector<StackAssembly::Instruction> &t_program);

        //Operations
        void add();
        void loadInteger(const short &t_value);
        void localToStack(const short &t_offset);
        void stackToLocal(const short &t_offset);
        void allocate(const short &t_amount);

        //Static private functions
        static std::vector<std::string> splitString(const std::string &t_str, const char &t_split_char);
        static std::string extractArg(const std::string &t_str);
        static short resolveArg(const std::string &t_str, const std::map<std::string, short> &t_jumps);

    public:
        void execute();

        //Static functions
        static StackCpu parse(const std::string &t_program);
    };

}


#endif //STECKASSEMBLER_STACKCPU_H
