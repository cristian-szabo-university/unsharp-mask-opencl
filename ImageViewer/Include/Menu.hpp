#pragma once

class Menu
{
public:

    Menu();

    Menu(const std::string& name);

    void addOption(
        const std::string& description,
        std::function<void()> action,
        std::function<bool()> pred = [] { return false; });

    void display(bool clear_screen);

    std::future<std::uint32_t> askInput();

    void runCommand(std::uint32_t cmd_id);

private:

    std::string name;

    std::vector<std::string> opts;

    std::vector<std::function<void()>> actions;

    std::vector<std::function<bool()>> preds;

};
