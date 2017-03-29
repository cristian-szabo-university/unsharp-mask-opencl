const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

inline float4 pixel_average(const int2 coord, image2d_t input)
{
    float4 clr_avrg = (float4)(0.0f);
    
    for (int y = -HALF_FILTER_SIZE; y <= HALF_FILTER_SIZE; y++)
	{
		for (int x = -HALF_FILTER_SIZE; x <= HALF_FILTER_SIZE; x++)
		{
            clr_avrg += read_imagef(input, smp, coord + (int2)(x, y));
        }
    }
    
    return (clr_avrg / (float4)(FILTER_AREA));
}

__kernel void blur_filter(image2d_t input, image2d_t output)
{
    const int2 coord = (int2)(get_global_id(0), get_global_id(1));
    
    const float4 result = pixel_average(coord, input);
    
    write_imagef(output, coord, clr_avrg);
}

__kernel void sharp_filter(image2d_t orig_img, image2d_t blur_img, image2d_t output)
{
    const int2 coord = (int2)(get_global_id(0), get_global_id(1));
    
    const float4 orig_clr = read_imagef(orig_img, smp, coord);
    const float4 blur_clr = pixel_average(coord, blur_img);
    
    float4 result = orig_clr * (float4)(ALPHA) + blur_clr * (float4)(BETA) + (float4)(GAMMA);
    result = clamp(result, (float4)(0.0f), (float4)(1.0f));
    
    write_imagef(output, coord, result);
}