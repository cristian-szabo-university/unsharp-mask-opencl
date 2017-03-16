#include "Main.hpp"

#include "Program.hpp"

int main(int argc, char** argv)
{
    int error_code;

    try
    {
        Program app({ argv + 1, argv + argc });

        error_code = app.run();
    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;

        error_code = -1;
    }

    return error_code;
}
