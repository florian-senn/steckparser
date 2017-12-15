#include <iostream>
#include <string>
#include "StackCpu.h"


int main() {
    std::string program1 = "ALLOC 1\n"
            "LDI 0\n"
            "STS 1\n"
            "from:\n"
            "LDS 1\n"
            "OUT\n"
            "LDS 1\n"
            "LDI 1\n"
            "ADD\n"
            "STS 1\n"
            "JUMP from";

    std::string program2 = "IN\n"
            "LDI fak\n"
            "CALL 1\n"
            "OUT\n"
            "LDI 0\n"
            "HALT\n"
            "\n"
            "// 1 Argument (n)\n"
            "fak:\n"
            "// Lokale variable (x)\n"
            "ALLOC 1\n"
            "// Wir setzen x = 1\n"
            "LDI 1\n"
            "STS 1\n"
            "// Ist n == 1?\n"
            "LDS 0\n"
            "LDI 1\n"
            "// Abbruchbedingung erfüllt?\n"
            "JE end\n"
            "// Rekursiver Aufruf mit n-1\n"
            "LDI 1\n"
            "LDS 0\n"
            "SUB\n"
            "LDI fak\n"
            "CALL 1\n"
            "// n*fak(n-1)\n"
            "LDS 0\n"
            "MUL\n"
            "STS 1\n"
            "end:\n"
            "LDS 1\n"
            "// Wir geben ein Argument und eine lokale Variable frei\n"
            "RETURN 2";

    auto cpu1 = StackCpuNS::StackCpu::parse(program1);
    auto cpu2 = StackCpuNS::StackCpu::parse(program2);

    cpu2.execute();

    return 0;
}