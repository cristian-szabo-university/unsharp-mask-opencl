#include "Program.hpp"

Program::Program(std::vector<std::string> cli)
{
    args = docopt::docopt(R"(
    Usage:
        projectapp.exe (-o OPERATION -a FIRST -b SECOND)

    Options:
        -o OPERATION --op=OPERATION     specify operation
        -a FIRST --first=FIRST          specify first operand
        -b SECOND --second=SECOND       specify secodn operand
    )", cli, true, "ProjectApp 1.0.0");
}

Program::~Program()
{
    args.clear();
}

int Program::run()
{
    int a = args["--first"].asLong();
    int b = args["--second"].asLong();
    std::string op = args["--op"].asString();

    int result;
    std::string sign;
    Calculator calc(a, b);

    if (op == "mul")
    {
        sign = "x";
        result = calc.mul();
    }
    else if (op == "div")
    {
        sign = ":";
        result = calc.div();
    }
    else if (op == "sum")
    {
        sign = "+";
        result = calc.sum();
    }
    else if (op == "diff")
    {
        sign = "-";
        result = calc.diff();
    }
    else
    {
        throw std::runtime_error("Invalid operation!");
    }

    std::cout << a << " " << sign << " " << b << " = " << result << std::endl;

    calc.destroy();

    return 0;
}
