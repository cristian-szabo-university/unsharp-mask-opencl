inline float3 pixel_average(const int mx, const int my, __global uchar* input, const int width, const int height)
{
    float3 sum = (float3)(0.0f);

    for (int y = -HALF_FILTER_SIZE; y <= HALF_FILTER_SIZE; ++y)
	{
        const int cy = clamp(my + y, 0, height - 1);
        
        const int row_idx = cy * width;
        
		for (int x = -HALF_FILTER_SIZE; x <= HALF_FILTER_SIZE; ++x)
		{
            const int cx = clamp(mx + x, 0, width - 1);
            
            sum += convert_float3(vload3(row_idx + cx, input));
        }
    }
    
    return (sum / (float3)(FILTER_AREA));
}

__kernel void blur_filter_fast(__global uchar* input, __global uchar* output, const int width, const int height)
{    
    const int x = get_global_id(0);
    const int y = get_global_id(1);  
    const int idx = y * width + x;

    const float3 result = pixel_average(x, y, input, width, height);
    
    vstore3(convert_uchar3(result), idx, output);
}

__kernel void sharp_filter_fast(__global uchar* orig_img, __global uchar* blur_img, __global uchar* output, const int width, const int height)
{    
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    const int idx = y * width + x;
      
    const float3 orig_clr = convert_float3(vload3(idx, orig_img));
    const float3 blur_clr = pixel_average(x, y, blur_img, width, height);
    
    float3 result = orig_clr * (float3)(ALPHA) + blur_clr * (float3)(BETA) + (float3)(GAMMA);
    result = clamp(result, (float3)(0.0f), (float3)(255.0f));
    
    vstore3(convert_uchar3(result), idx, output);
}
