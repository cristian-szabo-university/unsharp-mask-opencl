#pragma once

#include "ImageProcess\ParallelSharpProcess.hpp"

class UNSHARP_MASK_PUBLIC ParallelBlurSharpProcess : public ParallelSharpProcess
{
public:

    ParallelBlurSharpProcess(
        cl::Context context, std::uint32_t radius, 
        float alpha, float beta, float gamma,
        bool bandwith_optimisation);

    virtual ~ParallelBlurSharpProcess();

    virtual bool create() override;

protected:

    virtual ProgramEntry getProgramSource() override;

    virtual std::uint64_t onApplyFilter(std::shared_ptr<ImageFilter> filter, const PPM& image) override;

private:

    PPM::Format saved_format;

    bool bandwith_optimisation;

};
