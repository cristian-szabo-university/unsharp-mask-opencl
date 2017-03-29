#pragma once

#include "PPM.hpp"
#include "App.hpp"

class ImageProcess;

namespace docopt
{
    struct value;
}

class ImageViewer : public App
{
public:

    ImageViewer(std::vector<std::string> cli);

    virtual ~ImageViewer();

    virtual bool onInit() override;

    virtual bool onUpdate(double delta_time) override;

    virtual void onRender() override;

    virtual void onResize(int width, int height) override;

    virtual void onKey(int key, int scancode, int action, int mod) override;

    virtual const std::string & getName() override;

    virtual std::uint32_t getWidth() override;

    virtual std::uint32_t getHeight() override;

private:

    static const std::string name;

    bool running;

    std::map<std::string, docopt::value> args;

    std::uint32_t radius;

    std::string input_filename;

    std::string output_filename;

    PPM image;

    std::uint32_t glTexId;

    std::shared_ptr<ImageProcess> proc;

};
