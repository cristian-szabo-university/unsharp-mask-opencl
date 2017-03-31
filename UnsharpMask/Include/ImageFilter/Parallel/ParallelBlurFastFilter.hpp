#pragma once

#include "ImageFilter\ImageFilter.hpp"

class UNSHARP_MASK_PUBLIC ParallelBlurFastFilter : public ImageFilter
{
public:

    ParallelBlurFastFilter(bool original = false);

    virtual ~ParallelBlurFastFilter();

protected:

    virtual bool onLoad(std::shared_ptr<ImageProcess> proc) override;

    virtual void onBefore(std::uint32_t glTexId) override;

    virtual std::uint64_t onApply(const PPM& image) override;

    virtual void onAfter() override;

private:

    cl::Context context;

    cl::CommandQueue queue;

    cl::Kernel kernel;

    cl::Buffer output;

    bool original;

};
