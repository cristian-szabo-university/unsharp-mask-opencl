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

inline float3 pixel_average_cache(const int mx, const int my, __local float* input, const int width, const int height)
{
    float3 sum = (float3)(0.0f);

    for (int y = -HALF_FILTER_SIZE; y <= HALF_FILTER_SIZE; ++y)
	{
        const int cy = clamp(my + y, 0, height - 1);
        
        const int row_idx = cy * width;
        
		for (int x = -HALF_FILTER_SIZE; x <= HALF_FILTER_SIZE; ++x)
		{
            const int cx = clamp(mx + x, 0, width - 1);
            
            sum += vload3(row_idx + cx, input);
        }
    }
    
    return (sum / (float3)(FILTER_AREA));
}

inline float3 pixel_average_simple(const int mx, const int my, __global uchar* input, __local float3 input[16][16], const int width, const int height)
{
    float3 sum = (float3)(0.0f);

    for (int y = my - HALF_FILTER_SIZE; y <= my + HALF_FILTER_SIZE; ++y)
	{
        const int cy = clamp(y, 0, height - 1);
        
		for (int x = mx - HALF_FILTER_SIZE; x <= mx + HALF_FILTER_SIZE; ++x)
		{
            const int cx = clamp(x, 0, width - 1);
            
            if (x > HALF_FILTER_SIZE && y)
            {
                sum += input[cx][cy];
            }
            else
            {
                
            }
        }
    }
    
    return (sum / (float3)(FILTER_AREA));
}

#define MAX_JOBS_CACHE (8)
#define LOCAL_IMAGE_SIZE (16 + HALF_FILTER_SIZE * 2)
#define MAX_ITEMS_REQ (LOCAL_IMAGE_SIZE * (LOCAL_IMAGE_SIZE / MAX_JOBS_CACHE))

inline void pixel_load_cache(__global uchar* input, __local float* cache, const int width, const int height)
{
    int2 wi0Coord = (int2)(get_group_id(0) * get_local_size(0), get_group_id(1) * get_local_size(1));

    int flatLocal = get_local_id(1) * 16 + get_local_id(0);

    if (flatLocal < MAX_ITEMS_REQ)
    {
        int i = flatLocal % LOCAL_IMAGE_SIZE;
        int j = flatLocal / LOCAL_IMAGE_SIZE;

        for (int k = 0; k < MAX_JOBS_CACHE; k++)
        {
            const int coord = (wi0Coord.y + j + k) * width + wi0Coord.x + i;
        
            const float3 tmp = convert_float3(vload3(coord, input));
        
            vstore3(tmp, (j + k) * LOCAL_IMAGE_SIZE + i, cache);
        }
    }
    
    barrier(CLK_LOCAL_MEM_FENCE);
}

__attribute__((vec_type_hint(uchar3)))
__attribute__((reqd_work_group_size(16, 16, 1))) 
__kernel void blur_filter_fast(__global uchar* input, __global uchar* output, const int width, const int height)
{    
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    const int idx = y * width + x;
    const int xl = get_local_id(0);
    const int yl = get_local_id(1);    

    __local float3 cache[16][16];
    
    cache[xl][yl] = convert_float3(vload3(idx, input));
    
    barrier(CLK_LOCAL_MEM_FENCE);
    
    //pixel_load_cache(input, cache, width, height);
  
    const float3 result = pixel_average_simple(x, y, input, cache, width, height);

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
