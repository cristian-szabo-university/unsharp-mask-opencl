#include "Config.hpp"

#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include "Program.hpp"

#include "App.hpp"

Program::Program() : ready(false)
{
}

Program::~Program()
{
    if (isReady())
    {
        destroy();
    }
}

bool Program::create()
{
    if (isReady())
    {
        return false;
    }

    if (!glfwInit())
    {
        std::cerr << "Failed to initialise GLFW library!\n";

        return false;
    }

    ready = !ready;

    return true;
}

bool Program::isReady()
{
    return ready;
}

bool Program::execute(std::shared_ptr<App> app)
{
    if (!isReady() || !app)
    {
        return false;
    }

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(
        app->getWidth(), app->getHeight(), 
        app->getName().c_str(), 
        NULL, NULL);

    if (!window)
    {
        std::cerr << "Failed to create window!\n";

        return false;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialise GLEW library!\n";

        return false;
    }

    glfwSetWindowUserPointer(window, app.get());
    glfwSetWindowCloseCallback(window, window_close_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    if (!app->onInit())
    {
        std::cerr << "Failed to initialise application!\n";

        return false;
    }

    glfwMakeContextCurrent(NULL);

    apps.insert(std::make_pair(app, window));

    return true;
}

void Program::loop()
{
    std::size_t window_active = apps.size();

    while (window_active)
    {
        for (auto& app : apps)
        {
            if (glfwWindowShouldClose(app.second))
            {
                continue;
            }

            glfwMakeContextCurrent(app.second);

            if (glfwGetWindowAttrib(app.second, GLFW_VISIBLE) == GLFW_FALSE)
            {
                glfwShowWindow(app.second);
            }

            if (!app_process(app.first))
            {
                glfwSetWindowShouldClose(app.second, GLFW_TRUE);
            }
        }

        for (auto& app : apps)
        {
            glfwSwapBuffers(app.second);
        }

        glfwPollEvents();

        window_active = std::count_if(apps.begin(), apps.end(),
            [](auto app)
        {
            return !glfwWindowShouldClose(app.second);
        });
    }
}

bool Program::destroy()
{
    if (!isReady())
    {
        return false;
    }

    for (auto& app : apps)
    {
        app.first->onClose();

        glfwDestroyWindow(app.second);
    }

    glfwTerminate();

    apps.clear();

    ready = !ready;

    return true;
}

bool Program::app_process(std::shared_ptr<App> app)
{
    bool result = true;

    try
    {
        if (app->onUpdate(glfwGetTime()))
        {
            app->onRender();
        }
        else
        {
            result = false;
        }
    }
    catch (std::exception& ex)
    {
        std::cerr << "Exception: " << ex.what() << std::endl;

        result = false;
    }

    return result;
}

void Program::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));

    app->onKey(key, scancode, action, mods);
}

void Program::framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));

    app->onResize(width, height);
}

void Program::window_close_callback(GLFWwindow * window)
{
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

/*
void Program::create(std::vector<std::string> cli, GLFWwindow* window)
{
    this->window = window;

    auto args = docopt::docopt(R"(
    Usage:
        image_viewer (serial|parallel) <input_file> <output_file> [--radius=<value>]
        image_viewer (-h | --help)
        image_viewer --version

    Options:
        --help      Show this screen.
        --version   Show version.
    )", cli, true, "ImageViewer 1.0.0");

    std::string input_filename = args["<input_file>"].asString();
    std::string output_filename = args["<output_file>"].asString();

    if (args["serial"].asBool())
    {

    }
    else if (args["parallel"].asBool())
    {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        if (platforms.size() == 0)
        {
            throw std::runtime_error("No OpenCL platform found!");
        }

        const auto hGLRC = wglGetCurrentContext();
        const auto hDC = wglGetCurrentDC();

        cl_context_properties properties[] =
        {
            CL_CONTEXT_PLATFORM, 0,
            CL_GL_CONTEXT_KHR, (cl_context_properties)hGLRC,
            CL_WGL_HDC_KHR, (cl_context_properties)hDC,
            0
        };

        cl::Context context;

        for (auto& platform : platforms)
        {
            properties[1] = (cl_context_properties)(platform());

            try
            {
                context = cl::Context(CL_DEVICE_TYPE_GPU, properties);
            }
            catch (cl::Error& err)
            {
                throw err;
            }
        }

        std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
    }

    std::ifstream first_file(input_filename, std::ios::in);
    if (!first_file.is_open())
    {
        throw std::runtime_error("Image not found!");
    }

    first_file >> image;



    onResizeEvent(1280, 720);
}

void Program::render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, proc->getTexture());
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);
        glTexCoord2d(0.0, 0.0); glVertex2d(0.0, 0.0);
        glTexCoord2d(1.0, 0.0); glVertex2d(1.0, 0.0);
        glTexCoord2d(1.0, 1.0); glVertex2d(1.0, 1.0);
        glTexCoord2d(0.0, 1.0); glVertex2d(0.0, 1.0);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Program::destroy()
{
}

void Program::onResizeEvent(int width, int height)
{
    double left, right, bottom, top;
    double center, scale;

    double width_ratio = 1.0 / (double)width;
    double height_ratio = 1.0 / (double)height;

    if (width_ratio < height_ratio)
    {
        scale = height_ratio / width_ratio;
        center = 0.5 * scale;

        left = 0.5 - center;
        right = 0.5 + center;
        bottom = 0.0;
        top = 1.0;
    }
    else
    {
        scale = width_ratio / height_ratio;
        center = 0.5 * scale;

        left = 0.0;
        right = 1.0;
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

void Program::onKeyEvent(int key, int scancode, int action, int mod)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_FALSE);
    }
}

int Program::run(GLFWwindow* window)
{
    cl_int err = CL_SUCCESS;

    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    if (platforms.size() == 0) 
    {
        throw std::runtime_error("No OpenCL platform found!");
    }

    cl::Platform platform = platforms.at(1);
    std::cout << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
    
    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

    if (devices.size() == 0)
    {
        throw std::runtime_error("No OpenCL device found!");
    }

    std::ifstream first_file("lena.ppm", std::ios::in);
    if (!first_file.is_open())
    {
        throw std::runtime_error("Image not found!");
    }

    PPM first_image;
    first_file >> first_image;
    first_image.convert(PPM::Format::RGBA);

    cl::Device device = devices.at(0);
    std::cout << device.getInfo<CL_DEVICE_NAME>() << std::endl;

    cl::Context context({ device });

    std::vector<cl::ImageFormat> formats;
    context.getSupportedImageFormats(CL_MEM_READ_ONLY, CL_MEM_OBJECT_IMAGE2D, &formats);

    for (auto format : formats)
    {
        if (format.image_channel_order == CL_RGBA && format.image_channel_data_type == CL_UNSIGNED_INT8)
        {
            int x = 5;
        }
    }
  
    cl::Program::Sources source;
    source.push_back(std::make_pair(Kernel.getData().c_str(), (std::size_t)Kernel.getData().size()));

    cl::Program program = cl::Program(context, source);

    std::stringstream ss;
    ss << "-DHALF_FILTER_SIZE=4 ";
    ss << "-DFILTER_SIZE=9 ";
    ss << "-DFILTER_AREA=81.0f ";
    ss << "-DWIDTH=512 ";
    ss << "-DHEIGHT=512 ";
    ss << "-DALPHA=1.5f ";
    ss << "-DBETA=-0.5f ";
    ss << "-DGAMMA=0.0f ";

    try
    {
        program.build(ss.str().c_str());
    }
    catch (cl::Error& ex)
    {
        if (ex.err() != CL_BUILD_PROGRAM_FAILURE)
        {
            throw ex;
        }

        std::string build_log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);

        throw std::runtime_error(build_log);
    }

    cl_build_status status = program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(device);

    if (status != CL_BUILD_SUCCESS)
    {
        throw std::runtime_error("Program build failed!");
    }
    
    cl::Event event;
    cl::Kernel kernel_blur(program, "gaussian_blur", &err);
    cl::Kernel kernel_usharp_mask(program, "unsharp_mask_fast", &err);

    cl::CommandQueue queue(context, device, 0, &err);
    
    std::size_t size = first_image.getSize();
    cl::Buffer orig_img(context, CL_MEM_READ_ONLY, size);
    cl::Buffer blur_img1(context, CL_MEM_READ_WRITE, size);
    cl::Buffer blur_img2(context, CL_MEM_READ_WRITE, size);
    cl::Buffer blur_img3(context, CL_MEM_READ_WRITE, size);
    cl::Buffer sharp_img(context, CL_MEM_WRITE_ONLY, size);

    queue.enqueueWriteBuffer(orig_img, CL_BLOCKING, 0, size, first_image.getData());

    kernel_blur.setArg(0, orig_img);
    kernel_blur.setArg(1, blur_img1);

    queue.enqueueNDRangeKernel(
        kernel_blur,
        cl::NullRange,
        cl::NDRange(512, 512),
        cl::NDRange(16, 16),
        NULL, &event);
    
    event.wait();
   
    kernel_blur.setArg(0, blur_img1);
    kernel_blur.setArg(1, blur_img2);

    queue.enqueueNDRangeKernel(
        kernel_blur,
        cl::NullRange,
        cl::NDRange(512, 512),
        cl::NDRange(16, 16),
        NULL, &event);

    event.wait();

    kernel_blur.setArg(0, blur_img2);
    kernel_blur.setArg(1, blur_img3);

    queue.enqueueNDRangeKernel(
        kernel_blur,
        cl::NullRange,
        cl::NDRange(512, 512),
        cl::NDRange(16, 16),
        NULL, &event);

    event.wait();

    kernel_usharp_mask.setArg(0, orig_img);
    kernel_usharp_mask.setArg(1, blur_img3);
    kernel_usharp_mask.setArg(2, sharp_img);

    queue.enqueueNDRangeKernel(
        kernel_usharp_mask,
        cl::NullRange,
        cl::NDRange(512, 512),
        cl::NDRange(16, 16),
        NULL, &event);

    event.wait();
    
   
    cl::size_t<3> origin;
    origin[0] = 0;
    origin[1] = 0;
    origin[2] = 0;

    cl::size_t<3> region;
    region[0] = first_image.getWidth();
    region[1] = first_image.getHeight();
    region[2] = 1;

    cl::Image2D orig_img(context,
        CL_MEM_READ_ONLY,
        cl::ImageFormat(CL_RGBA, CL_UNORM_INT8),
        first_image.getWidth(), first_image.getHeight());

    queue.enqueueWriteImage(orig_img, CL_BLOCKING, origin, region, 0, 0, first_image.getData());

    cl::Image2D blur_img1(context,
        CL_MEM_READ_WRITE,
        cl::ImageFormat(CL_RGBA, CL_UNORM_INT8),
        first_image.getWidth(), first_image.getHeight());

    cl::Image2D blur_img2(context,
        CL_MEM_READ_WRITE,
        cl::ImageFormat(CL_RGBA, CL_UNORM_INT8),
        first_image.getWidth(), first_image.getHeight());

    cl::Image2D blur_img3(context,
        CL_MEM_READ_WRITE,
        cl::ImageFormat(CL_RGBA, CL_UNORM_INT8),
        first_image.getWidth(), first_image.getHeight());

    cl::Image2D sharp_img(context,
        CL_MEM_WRITE_ONLY,
        cl::ImageFormat(CL_RGBA, CL_UNORM_INT8),
        first_image.getWidth(), first_image.getHeight());

    std::vector<float> gaussian_weights = createGaussianFilter(4, 2.0f);

    cl::Buffer filter(context, CL_MEM_READ_ONLY, sizeof(float) * gaussian_weights.size());

    queue.enqueueWriteBuffer(filter, CL_BLOCKING, 0, sizeof(float) * gaussian_weights.size(), gaussian_weights.data());

    kernel_blur.setArg(0, orig_img);
    kernel_blur.setArg(1, filter);
    kernel_blur.setArg(2, blur_img1);

    queue.enqueueNDRangeKernel(
        kernel_blur,
        cl::NullRange,
        cl::NDRange(512, 512),
        cl::NDRange(16, 16),
        NULL, &event);

    event.wait();

    kernel_usharp_mask.setArg(0, orig_img);
    kernel_usharp_mask.setArg(1, blur_img1);
    kernel_usharp_mask.setArg(2, sharp_img);

    queue.enqueueNDRangeKernel(
        kernel_usharp_mask,
        cl::NullRange,
        cl::NDRange(512, 512),
        cl::NDRange(16, 16),
        NULL, &event);

    event.wait();
    

    kernel_blur.setArg(0, orig_img);
    kernel_blur.setArg(1, blur_img1);

    queue.enqueueNDRangeKernel(
        kernel_blur,
        cl::NullRange,
        cl::NDRange(512, 512),
        cl::NDRange(16, 16),
        NULL, &event);

    event.wait();

    kernel_blur.setArg(0, blur_img1);
    kernel_blur.setArg(1, blur_img2);

    queue.enqueueNDRangeKernel(
        kernel_blur,
        cl::NullRange,
        cl::NDRange(512, 512),
        cl::NDRange(16, 16),
        NULL, &event);

    event.wait();

    kernel_blur.setArg(0, blur_img2);
    kernel_blur.setArg(1, blur_img3);

    queue.enqueueNDRangeKernel(
        kernel_blur,
        cl::NullRange,
        cl::NDRange(512, 512),
        cl::NDRange(16, 16),
        NULL, &event);

    event.wait();

    kernel_usharp_mask.setArg(0, orig_img);
    kernel_usharp_mask.setArg(1, blur_img3);
    kernel_usharp_mask.setArg(2, sharp_img);

    queue.enqueueNDRangeKernel(
        kernel_usharp_mask,
        cl::NullRange,
        cl::NDRange(512, 512),
        cl::NDRange(16, 16),
        NULL, &event);

    event.wait();
 *
    PPM second_image;
    second_image.create(512, 512, PPM::Format::RGBA);

    //queue.enqueueReadBuffer(sharp_img, CL_BLOCKING, 0, second_image.getSize(), second_image.getData());
    queue.enqueueReadImage(blur_img1, CL_BLOCKING, origin, region, 0, 0, second_image.getData());
    
    second_image.convert(PPM::Format::RGB);

    std::ofstream file("output.ppm");
    file << second_image;
    file.close();

    return 0;
}

std::vector<float> Program::createGaussianFilter(std::int32_t radius, float sigma)
{
    std::vector<float> result;

    std::size_t filter_size = radius * 2 + 1;
    result.resize(filter_size * filter_size);

    std::size_t index = 0;
    float sum = 0.0f;

    for (std::int32_t y = -radius; y <= radius; y++)
    {
        for (std::int32_t x = -radius; x <= radius; x++, index++)
        {
            float temp = std::exp(-(std::powf((float)x, 2.0f) + std::powf((float)y, 2.0f) / (2.0f * std::powf(sigma, 2.0f))));

            result[index] = temp;

            sum += temp;
        }
    }

    for (auto& mask : result)
    {
        mask /= sum;
    }

    return result;
}
*/
