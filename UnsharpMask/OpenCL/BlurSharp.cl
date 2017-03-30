const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

inline float4 pixel_average(const int2 coord, __read_only image2d_t input)
{
    float4 accum = (float4)(0.0f);
    
    for (int y = -HALF_FILTER_SIZE; y <= HALF_FILTER_SIZE; y++)
	{
		for (int x = -HALF_FILTER_SIZE; x <= HALF_FILTER_SIZE; x++)
		{
            accum += read_imagef(input, smp, coord + (int2)(x, y));
        }
    }
    
    return (accum / (float4)(FILTER_AREA));
}

__kernel void blur_filter(__read_only image2d_t input, __write_only image2d_t output)
{
    const int2 coord = (int2)(get_global_id(0), get_global_id(1));
    
    const float4 result = pixel_average(coord, input);
    
    write_imagef(output, coord, result);
}

__kernel void sharp_filter(__read_only image2d_t orig_img, __read_only image2d_t blur_img, __write_only image2d_t output)
{
    const int2 coord = (int2)(get_global_id(0), get_global_id(1));
    
    const float4 orig_clr = read_imagef(orig_img, smp, coord);
    const float4 blur_clr = pixel_average(coord, blur_img);
    
    float4 result = orig_clr * (float4)(ALPHA) + blur_clr * (float4)(BETA) + (float4)(GAMMA);
    result = clamp(result, (float4)(0.0f), (float4)(1.0f));
    
    write_imagef(output, coord, result);
}