#include <emu.h>
#include <iostream>


#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>

int main(int argc, char **argv)
{
    plog::init(plog::debug, "log.txt");

    PLOGD << "Test";

    const int runResult = GB::EMU::GetEMU()->Run(argc, argv);

    return runResult;
}
