#pragma once

#include "ImageProcess\ImageProcess.hpp"

class UNSHARP_MASK_PUBLIC SharpProcess : public ImageProcess
{
public:

    SharpProcess(std::uint32_t radius, float alpha, float beta, float gamma);

    virtual ~SharpProcess();

    void setRadius(std::uint32_t radius);

    std::uint32_t getRadius();

    float getAlpha();

    float getBeta();

    float getGamma();

    virtual bool destroy() override;

protected:

    virtual void onBeforeExecute(PPM& image) override;

    virtual void onRadiusChange();

private:

    std::uint32_t radius;

    float alpha;

    float beta;

    float gamma;

};
