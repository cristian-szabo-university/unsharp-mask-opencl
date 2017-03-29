#pragma once

#include "ImageProcess\SharpProcess.hpp"

class ProgramEntry;

class UNSHARP_MASK_PUBLIC ParallelSharpProcess : public SharpProcess
{
public:

    ParallelSharpProcess(cl::Context context, std::uint32_t radius, float alpha, float beta, float gamma);

    virtual ~ParallelSharpProcess();

    cl::Program getProgram();

    cl::CommandQueue getQueue();

    virtual bool create() override;

protected:

    void attachBuildOption(std::pair<std::string, std::string> opt);

    virtual ProgramEntry getProgramSource() = 0;

private:

    cl::Context context;

    cl::Program program;

    cl::CommandQueue queue;

    std::vector<std::string> build_opts;

};
