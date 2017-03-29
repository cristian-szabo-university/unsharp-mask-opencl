#include "UnsharpMask.hpp"

#include "docopt.h"
#include "GL\glew.h"
#include "GL\wglew.h"
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

bool ImageViewer::onInit()
{
    radius = args["--radius"].asLong();
    input_filename = args["<input_file>"].asString();
    output_filename = args["<output_file>"].asString();

    std::ifstream first_file(input_filename, std::ios::in);

    if (!first_file.is_open())
    {
        std::cerr << "File " << input_filename << " not found!\n";

        return false;
    }

    first_file >> image;

    if (args["serial"].asBool())
    {
        proc = std::make_shared<SerialBlurSharpProcess>(radius, 1.5f, -0.5f, 0.0f);
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

        cl::Context context(CL_DEVICE_TYPE_ALL, properties);
        /*
        for (auto& platform : platforms)
        {
            properties[1] = (cl_context_properties)(platform());

            try
            {
                context = cl::Context(CL_DEVICE_TYPE_GPU, properties);
            }
            catch (cl::Error& err)
            {
                std::cerr << "No device found for platform " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;

                continue;
            }

            if (context())
            {
                break;
            }
        }
        */
        //proc = std::make_shared<ParallelBlurSharpProcess>(context, radius, 1.5f, -0.5f, 0.0f, false);
        proc = std::make_shared<ParallelGaussianSharpProcess>(context, radius, 1.5f, -0.5f, 0.0f);
    }

    bool result;

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

    std::uint64_t exec_time = proc->execute(image);

    std::cout << "Time elapsed: " << exec_time / 1000 / 1000 << " ms." << std::endl;

    glGenTextures(1, &glTexId);

    glBindTexture(GL_TEXTURE_2D, glTexId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, image.getWidth(), image.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.getData());

    glBindTexture(GL_TEXTURE_2D, 0);

    return result;
}

bool ImageViewer::onUpdate(double delta_time)
{
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
