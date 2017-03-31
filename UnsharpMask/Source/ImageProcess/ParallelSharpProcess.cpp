#include "Config.hpp"

#include "ImageProcess\ParallelSharpProcess.hpp"

#include "ProgramEntry.hpp"

ParallelSharpProcess::ParallelSharpProcess(cl::Context context, std::uint32_t radius, float alpha, float beta, float gamma)
    : context(context), SharpProcess(radius, alpha, beta, gamma)
{
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

    context.getSupportedImageFormats(CL_MEM_READ_WRITE, CL_MEM_OBJECT_IMAGE2D, &formats);

    std::size_t filter_size = getRadius() * 2 + 1;

    attachBuildOption(std::make_pair("HALF_FILTER_SIZE", std::to_string(getRadius())));
    attachBuildOption(std::make_pair("FILTER_SIZE", std::to_string(filter_size)));
    attachBuildOption(std::make_pair("FILTER_AREA", std::to_string(std::pow(filter_size, 2))));

    attachBuildOption(std::make_pair("ALPHA", std::to_string(getAlpha())));
    attachBuildOption(std::make_pair("BETA", std::to_string(getBeta())));
    attachBuildOption(std::make_pair("GAMMA", std::to_string(getGamma())));

    std::vector<cl::Device> devices;
    context.getInfo(CL_CONTEXT_DEVICES, &devices);

    ProgramEntry& prog_src = getProgramSource();

    cl::Program::Sources source;
    source.push_back(std::make_pair(
        prog_src.getData().c_str(), 
        (std::size_t)prog_src.getData().size()));

    program = cl::Program(context, source);

    std::stringstream ss;

    for (auto& opt : build_opts)
    {
        ss << opt << " ";
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

    cl_build_status status = program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices[0]);

    if (status != CL_BUILD_SUCCESS)
    {
        throw std::runtime_error("Program build failed!");
    }

    queue = cl::CommandQueue(context, CL_QUEUE_PROFILING_ENABLE);

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

void ParallelSharpProcess::attachBuildOption(std::pair<std::string, std::string> opt)
{
    std::string buffer;

    if (opt.first.find('-') == std::string::npos)
    {
        buffer += "-D";
    }

    buffer += opt.first;

    if (!opt.second.empty())
    {
        buffer += "=";
    }
    
    buffer += opt.second;

    build_opts.push_back(buffer);
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
