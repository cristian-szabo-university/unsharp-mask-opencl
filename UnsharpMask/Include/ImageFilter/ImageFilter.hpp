#pragma once

#include "PPM.hpp"

class ImageProcess;

class UNSHARP_MASK_PUBLIC ImageFilter
{
public:

    virtual ~ImageFilter();

protected:

    virtual bool onLoad(std::shared_ptr<ImageProcess> proc) = 0;

    virtual void onBefore(std::uint32_t glTexId) = 0;

    virtual std::uint64_t onApply(const PPM& image) = 0;

    virtual void onAfter() = 0;

private:

    friend class ImageProcess;

};