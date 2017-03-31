#include "Config.hpp"

#include "Menu.hpp"

Menu::Menu()
{
}

Menu::Menu(const std::string & name)
    : name(name)
{
}

void Menu::addOption(const std::string & description, std::function<void()> action, std::function<bool()> pred)
{
    opts.push_back(description);
    actions.push_back(action);
    preds.push_back(pred);
}

void Menu::display(bool clear_screen)
{
    if (clear_screen)
    {
        system("cls");
    }

    std::cout << std::setfill('-') << std::setw(49) << '-' << std::endl;
    std::cout << name << std::endl;
    std::cout << std::setfill('-') << std::setw(49) << '-' << std::endl;
    std::cout << std::endl;

    for (std::uint32_t i = 0; i < opts.size(); i++)
    {
        std::string prefix = preds.at(i)() ? "->" : "";

        std::cout << "\t" << std::setfill(' ') << std::setw(3) << std::left << prefix;
        std::cout << "(" << i + 1 << ") " << opts.at(i) << std::endl;
    }

    std::cout << std::endl;
    std::cout << std::setfill('-') << std::setw(49) << '-' << std::endl;
}

std::future<std::uint32_t> Menu::askInput()
{
    return std::async(std::launch::async, 
        [&]() 
    {
        std::cout << std::endl;

        std::string input;
        std::uint32_t cmd_id;

        while (true)
        {
            std::cout << "Select option: ";

            std::cin >> input;

            if (input.size() > 1)
            {
                std::cout << "Invalid option! Try again.\n";

                continue;
            }

            cmd_id = std::stoi(input);

            if (cmd_id < 1 || cmd_id > opts.size())
            {
                std::cout << "Option not found! Try again.\n";

                continue;
            }

            break;
        }

        std::cout << std::endl;

        return cmd_id - 1;
    });
}

void Menu::runCommand(std::uint32_t cmd_id)
{
    actions.at(cmd_id)();
}
