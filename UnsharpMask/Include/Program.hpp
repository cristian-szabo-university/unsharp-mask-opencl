#pragma once

#include "Lib.hpp"

#include "docopt.h"

class Program
{
public:

    Program(std::vector<std::string> args);

    ~Program();

    int run();

private:

    std::map<std::string, docopt::value> args;

};
