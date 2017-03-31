#pragma once

#include "ImageProcess\ParallelSharpProcess.hpp"

class UNSHARP_MASK_PUBLIC ParallelBlurSharpFastProcess : public ParallelSharpProcess
{
public:

    ParallelBlurSharpFastProcess(
        cl::Context context, std::uint32_t radius, 
        float alpha, float beta, float gamma);

    virtual ~ParallelBlurSharpFastProcess();

    virtual bool create() override;

protected:

    virtual ProgramEntry getProgramSource() override;

    virtual void onBeforeExecute(PPM & image) override;

    virtual void onAfterExecute() override;

private:

    std::uint32_t glTexId;

};
