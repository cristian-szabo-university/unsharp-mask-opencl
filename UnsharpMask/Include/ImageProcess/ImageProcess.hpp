#pragma once

#include "PPM.hpp"

class ImageFilter;

class UNSHARP_MASK_PUBLIC ImageProcess : public std::enable_shared_from_this<ImageProcess>
{
public:

    ImageProcess();

    virtual ~ImageProcess();

    void attachFilter(std::shared_ptr<ImageFilter> filter);

    std::uint32_t getObjectGL();

    virtual bool create();

    bool isReady();

    virtual std::uint64_t execute(const PPM& image);

    virtual bool destroy();

protected:

    void setObjectGL(std::uint32_t objectId);

    virtual void onBeforeExecute(PPM& image);

    virtual void onAfterExecute();

    virtual void onBeforeFilter(std::shared_ptr<ImageFilter> filter, std::uint32_t objectId);

    virtual std::uint64_t onApplyFilter(std::shared_ptr<ImageFilter> filter, const PPM & image);

    virtual void onAfterFilter(std::shared_ptr<ImageFilter> filter);

private:

    bool ready;

    std::uint32_t objectId;

    std::vector<std::shared_ptr<ImageFilter>> filters;

};
