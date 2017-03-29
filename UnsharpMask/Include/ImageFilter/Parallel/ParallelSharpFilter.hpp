#pragma once

#include "ImageFilter\ImageFilter.hpp"

class UNSHARP_MASK_PUBLIC ParallelSharpFilter : public ImageFilter
{
public:

    ParallelSharpFilter();

    virtual ~ParallelSharpFilter();

protected:

    virtual bool onLoad(std::shared_ptr<ImageProcess> proc) override;

    virtual void onBefore(std::uint32_t glTexId) override;

    virtual std::uint64_t onApply(const PPM & image) override;

    virtual void onAfter() override;

};
