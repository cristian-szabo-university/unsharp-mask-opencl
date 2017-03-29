#include "Config.hpp"

#include "ImageProcess\Serial\SerialBlurSharpProcess.hpp"

#include "ImageFilter\Serial\SerialBlurFilter.hpp"
#include "ImageFilter\Serial\SerialSharpFilter.hpp"

#include "GL\glew.h"

SerialBlurSharpProcess::SerialBlurSharpProcess(std::uint32_t radius, float alpha, float beta, float gamma)
    : SharpProcess(radius, alpha, beta, gamma)
{
}

SerialBlurSharpProcess::~SerialBlurSharpProcess()
{
}

bool SerialBlurSharpProcess::create()
{
    if (isReady())
    {
        return false;
    }

    std::shared_ptr<ImageFilter> blur_filter = std::make_shared<SerialBlurFilter>();

    attachFilter(blur_filter);
    attachFilter(blur_filter);
    attachFilter(blur_filter);

    std::shared_ptr<ImageFilter> sharp_filter = std::make_shared<SerialSharpFilter>();

    attachFilter(sharp_filter);

    return SharpProcess::create();
}

