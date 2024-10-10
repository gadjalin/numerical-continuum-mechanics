#include <iostream>

#include "args.hpp"

int main(int argc, char* argv[])
{
    auto args = parse_command_line(argc, argv);
    std::printf("par file: %s\noutput file: %s\n", args.par_file.c_str(), args.output_file.c_str());

    return 0;
}

