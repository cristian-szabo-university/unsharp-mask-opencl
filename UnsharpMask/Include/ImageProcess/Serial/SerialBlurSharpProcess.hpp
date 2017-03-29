#pragma once

#include "ImageProcess\SharpProcess.hpp"

class UNSHARP_MASK_PUBLIC SerialBlurSharpProcess : public SharpProcess
{
public:

    SerialBlurSharpProcess(std::uint32_t radius, float alpha, float beta, float gamma);

    virtual ~SerialBlurSharpProcess();

    virtual bool create() override;

};
