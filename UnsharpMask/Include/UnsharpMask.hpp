#pragma once

#include "Config.hpp"

#include "PPM.hpp"

#include "ImageFilter\ImageFilter.hpp"

#include "ImageFilter\Serial\SerialBlurFilter.hpp"
#include "ImageFilter\Serial\SerialSharpFilter.hpp"

#include "ImageFilter\Parallel\ParallelBlurFastFilter.hpp"
#include "ImageFilter\Parallel\ParallelBlurFilter.hpp"
#include "ImageFilter\Parallel\ParallelGaussianSharpFilter.hpp"
#include "ImageFilter\Parallel\ParallelSharpFastFilter.hpp"
#include "ImageFilter\Parallel\ParallelSharpFilter.hpp"

#include "ImageProcess\ImageProcess.hpp"
#include "ImageProcess\ParallelSharpProcess.hpp"

#include "ImageProcess\Serial\SerialBlurSharpProcess.hpp"

#include "ImageProcess\Parallel\ParallelBlurSharpProcess.hpp"
#include "ImageProcess\Parallel\ParallelGaussianSharpProcess.hpp"

#include "Utility.hpp"
