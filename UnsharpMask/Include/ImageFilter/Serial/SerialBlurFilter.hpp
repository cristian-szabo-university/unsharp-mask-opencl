#pragma once

#include "ImageFilter\ImageFilter.hpp"

class UNSHARP_MASK_PUBLIC SerialBlurFilter : public ImageFilter
{
public:

    SerialBlurFilter();

    virtual ~SerialBlurFilter();

protected:

    virtual bool onLoad(std::shared_ptr<ImageProcess> proc) override;

    virtual void onBefore(std::uint32_t glTexId) override;

    virtual std::uint64_t onApply(const PPM& image) override;

    virtual void onAfter() override;

private:

    std::int32_t radius;

    PPM result;

    PPM previous;

    void pixelAverage(
        std::int32_t x, std::int32_t y, 
        std::int32_t width, std::int32_t height,
        std::uint8_t* input, std::uint8_t* output, 
        const std::int32_t channels, float samples);

};
