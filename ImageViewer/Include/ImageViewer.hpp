#pragma once

#include "PPM.hpp"
#include "App.hpp"
#include "Menu.hpp"

class SharpProcess;

namespace docopt
{
    struct value;
}

class ImageViewer : public App
{
public:

    ImageViewer(std::vector<std::string> cli);

    virtual ~ImageViewer();

    virtual bool onInit() override;

    virtual bool onUpdate(double delta_time) override;

    virtual void onRender() override;

    virtual void onResize(int width, int height) override;

    virtual void onKey(int key, int scancode, int action, int mod) override;

    virtual const std::string & getName() override;

    virtual std::uint32_t getWidth() override;

    virtual std::uint32_t getHeight() override;

private:

    enum MenuType
    {
        Main,
        Blur,
        Radius,
        Count
    };

    static const std::string name;

    std::atomic_bool running;

    std::map<std::string, docopt::value> args;

    std::uint32_t radius;

    std::string input_filename;

    std::string output_filename;

    PPM image;

    std::uint32_t glTexId;

    std::shared_ptr<SharpProcess> proc;

    cl::Context context;

    std::future<std::uint32_t> user_input;

    Menu active_menu;

    std::vector<Menu> menu_list;

    template<class T>
    bool isSharpProcess()
    {
        std::shared_ptr<T> cast_proc = std::dynamic_pointer_cast<T>(proc);

        if (!cast_proc)
        {
            return false;
        }

        return true;
    }

    template<class T, class... A>
    bool changeSharpProcess(A&&... args)
    {
        bool result;

        if (proc && proc->isReady())
        {
            proc->destroy();
        }

        proc = std::make_shared<T>(std::forward<A>(args)...);

        try
        {
            result = proc->create();
        }
        catch (cl::Error& ex)
        {
            std::cerr << "OpenCL Error: " << ex.err() << std::endl;
            std::cerr << ex.what() << std::endl;

            result = false;
        }
        catch (std::exception& ex)
        {
            std::cerr << "Exception: " << ex.what() << std::endl;

            result = false;
        }

        if (!result)
        {
            return false;
        }

        std::uint64_t time_exec = proc->execute(image);

        active_menu.display(true);

        return result;
    }

    void changeRadius(std::uint32_t radius);

    void changeMenu(MenuType type);

};
