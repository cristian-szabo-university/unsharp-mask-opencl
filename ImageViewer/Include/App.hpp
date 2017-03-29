#pragma once

struct GLFWwindow;

class App
{
public:

    virtual ~App();

    virtual const std::string& getName() = 0;

    virtual std::uint32_t getWidth() = 0;

    virtual std::uint32_t getHeight() = 0;

    virtual bool onUpdate(double delta_time);

    virtual void onRender();

    virtual bool onInit() = 0;

    virtual void onClose();

    virtual void onResize(int width, int height);

    virtual void onKey(int key, int scancode, int action, int mod);

};
