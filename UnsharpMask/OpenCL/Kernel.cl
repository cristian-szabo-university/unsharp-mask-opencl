__kernel void gaussian_blur(__read_only image2d_t input, __constant float* filter, __write_only image2d_t output)
{
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    
    const int2 coord = (int2)(get_global_id(0), get_global_id(1));
    
    size_t index = 0;
    float4 clr_sum = (float4)(0.0f);
    
    for (int y = -HALF_FILTER_SIZE; y <= HALF_FILTER_SIZE; y++)
	{
		for (int x = -HALF_FILTER_SIZE; x <= HALF_FILTER_SIZE; x++, index++)
		{
            clr_sum += read_imagef(input, smp, coord + (int2)(x, y)) * filter[index];
        }
    }
    
    write_imagef(output, coord, clr_sum);
}

__kernel void avrg_blur(__global uchar* input, __global uchar* output)
{    
    const int orig_x = get_global_id(0);
    const int orig_y = get_global_id(1);
    const int orig_idx = orig_y * WIDTH + orig_x;
  
    float3 clr_avrg = (float3)(0.0f);
    
    for (int y = -HALF_FILTER_SIZE; y <= HALF_FILTER_SIZE; y++)
	{
        const int cy = clamp(orig_y + y, 0, HEIGHT - 1);
        
        const int row_idx = cy * WIDTH;
        
		for (int x = -HALF_FILTER_SIZE; x <= HALF_FILTER_SIZE; x++)
		{
            const int cx = clamp(orig_x + x, 0, WIDTH - 1);
            
            clr_avrg += convert_float3(vload3(row_idx + cx, input));
        }
    }
       
    clr_avrg /= (float3)(FILTER_AREA);

    vstore3(convert_uchar3(clr_avrg), orig_idx, output);
}

__kernel void avrg_blur_fast(__read_only image2d_t input, __write_only image2d_t output)
{
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    
    const int2 coord = (int2)(get_global_id(0), get_global_id(1));
    
    float4 clr_avrg = (float4)(0.0f);
    
    for (int y = -HALF_FILTER_SIZE; y <= HALF_FILTER_SIZE; y++)
	{
		for (int x = -HALF_FILTER_SIZE; x <= HALF_FILTER_SIZE; x++)
		{
            clr_avrg += read_imagef(input, smp, coord + (int2)(x, y));
        }
    }
    
    clr_avrg /= (float4)(FILTER_AREA);
    
    write_imagef(output, coord, clr_avrg);
}

__kernel void unsharp_mask_fast(__read_only image2d_t orig_img, __read_only image2d_t blur_img, __write_only image2d_t output)
{
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    
    const int2 coord = (int2)(get_global_id(0), get_global_id(1));
    
    const float4 orig_clr = read_imagef(orig_img, smp, coord);
    const float4 blur_clr = read_imagef(blur_img, smp, coord);
    
    float4 result = orig_clr * (float4)(ALPHA) + blur_clr * (float4)(BETA) + (float4)(GAMMA);
    result = clamp(result, (float4)(0.0f), (float4)(1.0f));
    
    write_imagef(output, coord, result);
}

__kernel void unsharp_mask(__global uchar* orig_img, __global uchar* blur_img, __global uchar* output)
{    
    const int orig_x = get_global_id(0);
    const int orig_y = get_global_id(1);
    const int orig_idx = orig_y * WIDTH + orig_x;
    
    const float3 orig_clr = convert_float3(vload3(orig_idx, orig_img));
    const float3 blur_clr = convert_float3(vload3(orig_idx, blur_img));
    
    float3 result = orig_clr * (float3)(ALPHA) + blur_clr * (float3)(BETA) + (float3)(GAMMA);
    result = clamp(result, (float3)(0.0f), (float3)(255.0f));
    
    vstore3(convert_uchar3(result), orig_idx, output);
}
