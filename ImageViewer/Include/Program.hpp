#pragma once

class App;

class Program
{
public:

    Program();

    ~Program();

    bool create();

    bool isReady();

    bool execute(std::shared_ptr<App> app);

    void loop();

    bool destroy();

private:

    bool ready;

    std::map<std::shared_ptr<App>, GLFWwindow*> apps;

    bool app_process(std::shared_ptr<App> app);

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    static void window_close_callback(GLFWwindow* window);

};
