#include "Config.hpp"

#include "ImageProcess\ParallelSharpProcess.hpp"

#include "ProgramEntry.hpp"

ParallelSharpProcess::ParallelSharpProcess(cl::Context context, std::uint32_t radius, float alpha, float beta, float gamma)
    : context(context), SharpProcess(radius, alpha, beta, gamma)
{
    attachBuildOption("ALPHA", std::to_string(getAlpha()));
    attachBuildOption("BETA", std::to_string(getBeta()));
    attachBuildOption("GAMMA", std::to_string(getGamma()));

    attachBuildOption("-cl-fast-relaxed-math");
    attachBuildOption("-cl-mad-enable");
}

ParallelSharpProcess::~ParallelSharpProcess()
{
}

cl::Program ParallelSharpProcess::getProgram()
{
    return program;
}

cl::CommandQueue ParallelSharpProcess::getQueue()
{
    return queue;
}

bool ParallelSharpProcess::create()
{
    if (isReady())
    {
        return false;
    }

    if (!context())
    {
        return false;
    }

    context.getSupportedImageFormats(CL_MEM_READ_WRITE, CL_MEM_OBJECT_IMAGE2D, &formats);

    queue = cl::CommandQueue(context, CL_QUEUE_PROFILING_ENABLE);

    onRadiusChange();

    return ImageProcess::create();
}

bool ParallelSharpProcess::destroy()
{
    if (!isReady())
    {
        return false;
    }

    build_opts.clear();

    formats.clear();

    return SharpProcess::destroy();
}

void ParallelSharpProcess::attachBuildOption(std::string name, std::string value)
{
    std::string option;

    if (name.find('-') == std::string::npos)
    {
        option += "-D";
    }

    option += name;

    auto iter_opt = std::find_if(build_opts.begin(), build_opts.end(),
        [&](std::pair<std::string, std::string> opt)
    {
        return opt.first == option;
    });

    if (iter_opt != build_opts.end())
    {
        iter_opt->second = value;
    }
    else
    {
        build_opts.push_back(std::make_pair(option, value));
    } 
}

void ParallelSharpProcess::onRadiusChange()
{
    std::size_t filter_size = getRadius() * 2 + 1;

    attachBuildOption("HALF_FILTER_SIZE", std::to_string(getRadius()));
    attachBuildOption("FILTER_SIZE", std::to_string(filter_size));
    attachBuildOption("FILTER_AREA", std::to_string(std::pow(filter_size, 2)));

    program = buildProgram(context);

    SharpProcess::onRadiusChange();
}

bool ParallelSharpProcess::hasImageFormat(cl::ImageFormat format)
{
    auto iter_format = std::find_if(formats.begin(), formats.end(), 
        [&](const cl::ImageFormat& check)
    {
        return format.image_channel_order == check.image_channel_order &&
            format.image_channel_data_type == check.image_channel_data_type;
    });

    if (iter_format == formats.end())
    {
        return false;
    }

    return true;
}

cl::Program ParallelSharpProcess::buildProgram(cl::Context context)
{
    std::vector<cl::Device> devices;
    context.getInfo(CL_CONTEXT_DEVICES, &devices);

#ifdef _DEBUG
    std::string vendor = devices[0].getInfo<CL_DEVICE_VENDOR>();

    if (vendor.find("NVIDIA") != std::string::npos)
    {
        attachBuildOption("-cl-nv-verbose");
    }
#endif

    ProgramEntry& prog_src = getProgramSource();

    cl::Program::Sources source;
    source.push_back(std::make_pair(
        prog_src.getData().c_str(),
        (std::size_t)prog_src.getData().size()));

    cl::Program program(context, source);

    std::stringstream ss;

    for (auto& opt : build_opts)
    {
        std::string value;
        
        if (!opt.second.empty())
        {
            value += "=";
            value += opt.second;
        }

        ss << opt.first << value << " ";
    }

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

        std::string build_log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]);

        throw std::runtime_error(build_log);
    }

#ifdef _DEBUG
    std::string build_log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]);

    std::cout << build_log << std::endl;
#endif

    cl_build_status status = program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices[0]);

    if (status != CL_BUILD_SUCCESS)
    {
        throw std::runtime_error("Program build failed!");
    }

    return program;
}
