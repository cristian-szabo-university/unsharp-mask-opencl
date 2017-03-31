#include "UnsharpMask.hpp"

#include "docopt.h"
#include "GLFW\glfw3.h"

#include "ImageViewer.hpp"

const std::string ImageViewer::name = "ImageViewer";

ImageViewer::ImageViewer(std::vector<std::string> cli)
    : running(true)
{
    args = docopt::docopt(R"(
    Usage:
        image_viewer (serial|parallel) <input_file> <output_file> [--radius=<value>]
        image_viewer (-h | --help)
        image_viewer --version

    Options:
        --help      Show this screen.
        --version   Show version.
    )", cli, true, "ImageViewer 1.0.0");

    menu_list.resize(MenuType::Count);
}

ImageViewer::~ImageViewer()
{
}

const std::string& ImageViewer::getName()
{
    return name;
}

std::uint32_t ImageViewer::getWidth()
{
    return 1280;
}

std::uint32_t ImageViewer::getHeight()
{
    return 720;
}

void ImageViewer::changeRadius(std::uint32_t radius)
{
    this->radius = radius;

    proc->setRadius(radius);

    if (proc->isReady())
    {
        proc->destroy();
    }

    proc->create();

    proc->execute(image);

    active_menu.display(true);
}

void ImageViewer::changeMenu(MenuType type)
{
    active_menu = menu_list.at(type);

    active_menu.display(true);
}

bool ImageViewer::onInit()
{
    bool result = true;

    radius = args["--radius"].asLong();
    input_filename = args["<input_file>"].asString();
    output_filename = args["<output_file>"].asString();

    std::ifstream file(input_filename, std::ios::in);

    if (!file.is_open())
    {
        std::cerr << "File " << input_filename << " not found!\n";

        return false;
    }

    file >> image;

    if (args["serial"].asBool())
    {
        result = changeSharpProcess<SerialBlurSharpProcess>(radius, 1.5f, -0.5f, 0.0f);
    }
    else if (args["parallel"].asBool())
    {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        if (platforms.size() == 0)
        {
            std::cerr << "No OpenCL platform found!\n";

            return false;
        }

        const auto hGLRC = wglGetCurrentContext();
        const auto hDC = wglGetCurrentDC();

        cl_context_properties properties[] =
        {
            CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[0](),
            CL_GL_CONTEXT_KHR, (cl_context_properties)hGLRC,
            CL_WGL_HDC_KHR, (cl_context_properties)hDC,
            0
        };

        for (auto& platform : platforms)
        {
            properties[1] = (cl_context_properties)(platform());

            try
            {
                context = cl::Context(CL_DEVICE_TYPE_GPU, properties);
            }
            catch (cl::Error& ex)
            {
                if (ex.err() != CL_DEVICE_NOT_FOUND)
                {
                    std::cerr << "OpenCL Error: " << ex.what() << std::endl;
                    std::cerr << "Code: " << ex.err() << std::endl;

                    return false;
                }

                std::cerr << "No device found for platform " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;

                continue;
            }

            if (context())
            {
                break;
            }
        }

        result = changeSharpProcess<ParallelBlurSharpFastProcess>(context, radius, 1.5f, -0.5f, 0.0f);
    }

    if (!result)
    {
        return false;
    }

    glGenTextures(1, &glTexId);

    glBindTexture(GL_TEXTURE_2D, glTexId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, image.getWidth(), image.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.getData());

    glBindTexture(GL_TEXTURE_2D, 0);

    Menu menu_main = Menu("Sharpenning Image Tool");
    menu_main.addOption(
        "Change blur algorithm.",
        [&] { changeMenu(MenuType::Blur); });
    menu_main.addOption(
        "Adjust box linear filter size.",
        [&] { changeMenu(MenuType::Radius); });
    menu_main.addOption(
        "Refresh image.",
        [&] 
    { 
        std::uint64_t time_exec = proc->execute(image);

        std::cout << "Time generated: " << time_exec / 1000.0f / 1000.0f << " ms." << std::endl;
    });
    menu_main.addOption(
        "Save sharp image.",
        [&] 
    { 
        PPM output;

        glBindTexture(GL_TEXTURE_2D, proc->getObjectGL());

        int width, height, internalFormat;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);

        output.create(width, height, internalFormat);

        glGetTexImage(GL_TEXTURE_2D, 0, output.getGLFormat(), GL_UNSIGNED_BYTE, output.getData());

        glBindTexture(GL_TEXTURE_2D, 0);

        if (output.getFormat() == PPM::Format::RG ||
            output.getFormat() == PPM::Format::RGBA)
        {
            output.convert(PPM::Format::RGB);
        }

        std::ofstream file(output_filename, std::ios::out); 
        file << output;
    });
    menu_main.addOption(
        "Quit application.",
        [&] { running = false; });
    menu_list[MenuType::Main] = menu_main;

    Menu menu_radius = Menu("Select blur filter size (radius)");
    menu_radius.addOption(
        "3 pixels (5x5 box linear filter)",
        [&] { changeRadius(2); }, 
        [&] { return radius == 2; });
    menu_radius.addOption(
        "5 pixels (9x9 box linear filter)",
        [&] { changeRadius(4); }, 
        [&] { return radius == 4; });
    menu_radius.addOption(
        "7 pixels (13x13 box linear filter)",
        [&] { changeRadius(6); }, 
        [&] { return radius == 6; });
    menu_radius.addOption(
        "11 pixels (21x21 box linear filter)",
        [&] { changeRadius(10); }, 
        [&] { return radius == 10; });
    menu_radius.addOption(
        "13 pixels (25x25 box linear filter)",
        [&] { changeRadius(12); }, 
        [&] { return radius == 12; });
    menu_radius.addOption(
        "Go back to main menu.",
        [&] { changeMenu(MenuType::Main); });
    menu_list[MenuType::Radius] = menu_radius;

    Menu menu_blur = Menu("Select blur algorithm");
    menu_blur.addOption(
        "Average Blur - Serial (CPU)",
        [&] { changeSharpProcess<SerialBlurSharpProcess>(radius, 1.5f, -0.5f, 0.0f); },
        [&] { return isSharpProcess<SerialBlurSharpProcess>(); });
    menu_blur.addOption(
        "Average Blur - Image2D (GPU)",
        [&] { changeSharpProcess<ParallelBlurSharpProcess>(context, radius, 1.5f, -0.5f, 0.0f); },
        [&] { return isSharpProcess<ParallelBlurSharpProcess>(); });
    menu_blur.addOption(
        "Average Blur - Buffer (GPU Bandwidth Efficient)",
        [&] { changeSharpProcess<ParallelBlurSharpFastProcess>(context, radius, 1.5f, -0.5f, 0.0f); },
        [&] { return isSharpProcess<ParallelBlurSharpFastProcess>(); });
    menu_blur.addOption(
        "Gaussian Blur (GPU)",
        [&] { changeSharpProcess<ParallelGaussianSharpProcess>(context, radius, 1.5f, -0.5f, 0.0f); },
        [&] { return isSharpProcess<ParallelGaussianSharpProcess>(); });
    menu_blur.addOption(
        "Go back to main menu.",
        [&] { changeMenu(MenuType::Main); });
    menu_list[MenuType::Blur] = menu_blur;

    changeMenu(MenuType::Main);

    return true;
}

bool ImageViewer::onUpdate(double delta_time)
{
    if (user_input.valid())
    {
        std::future_status status = user_input.wait_for(std::chrono::nanoseconds(1));

        if (status == std::future_status::ready)
        {
            active_menu.runCommand(user_input.get());
        }
    }
    else
    {
        user_input = active_menu.askInput();
    }

    return running;
}

void ImageViewer::onRender()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, glTexId);

    glBegin(GL_QUADS);
        glTexCoord2d(0.0, 0.0); glVertex2d(0.0, 0.0);
        glTexCoord2d(1.0, 0.0); glVertex2d(1.0, 0.0);
        glTexCoord2d(1.0, 1.0); glVertex2d(1.0, 1.0);
        glTexCoord2d(0.0, 1.0); glVertex2d(0.0, 1.0);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, proc->getObjectGL());

    glBegin(GL_QUADS);
        glTexCoord2d(0.0, 0.0); glVertex2d(1.0, 0.0);
        glTexCoord2d(1.0, 0.0); glVertex2d(2.0, 0.0);
        glTexCoord2d(1.0, 1.0); glVertex2d(2.0, 1.0);
        glTexCoord2d(0.0, 1.0); glVertex2d(1.0, 1.0);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_TEXTURE_2D);

    glFlush();
}

void ImageViewer::onResize(int width, int height)
{
    double left, right, bottom, top;
    double center, scale;

    double width_ratio = 2.0 / (double)width;
    double height_ratio = 1.0 / (double)height;

    if (width_ratio < height_ratio)
    {
        scale = height_ratio / width_ratio;
        center = 1.0 * scale;

        left = 1.0 - center;
        right = 1.0 + center;
        bottom = 0.0;
        top = 1.0;
    }
    else
    {
        scale = width_ratio / height_ratio;
        center = 0.5 * scale;

        left = 0.0;
        right = 2.0;
        bottom = 0.5 - center;
        top = 0.5 + center;
    }

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(left, right, top, bottom, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void ImageViewer::onKey(int key, int scancode, int action, int mod)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        running = false;
    }
}
