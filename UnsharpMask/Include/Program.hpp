#pragma once

#include <vector>

class Program
{
public:

    Program(std::vector<std::string> args);

    ~Program();

    int run();

};
