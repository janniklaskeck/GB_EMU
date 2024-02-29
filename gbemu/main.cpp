#include <emu.h>
#include <iostream>

int main(int argc, char **argv)
{
    const int runResult = GB::EMU::GetEMU()->Run(argc, argv);

    return runResult;
}
