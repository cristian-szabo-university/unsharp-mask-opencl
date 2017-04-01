#define WORK_GROUP_X 8
#define WORK_GROUP_Y 4
#define PIXEL_WORK_ITEMS 8

#define LOCAL_IMAGE_WIDTH ((WORK_GROUP_X) + (HALF_FILTER_SIZE) * 2)
#define LOCAL_IMAGE_HEIGHT ((WORK_GROUP_Y) + (HALF_FILTER_SIZE) * 2)
#define LOCAL_IMAGE_SIZE (LOCAL_IMAGE_WIDTH * LOCAL_IMAGE_HEIGHT)

#define PIXEL_CACHE_STEP ((LOCAL_IMAGE_HEIGHT) / (PIXEL_WORK_ITEMS))
#define PIXEL_CACHE_JOBS (LOCAL_IMAGE_SIZE / (PIXEL_WORK_ITEMS))

inline float3 pixel_average(int2 lid, __local float3 rgb[LOCAL_IMAGE_WIDTH][LOCAL_IMAGE_HEIGHT])
{
    float3 sum = (float3)(0.0f);

    for (int y = 0; y < FILTER_SIZE; ++y)
	{
		for (int x = 0; x < FILTER_SIZE; ++x)
		{
            sum += rgb[lid.x + x][lid.y + y];
        }
    }
    
    return (sum / (float3)(FILTER_AREA));
}

inline void prefetch_pixels(int2 gid, int2 lid, __global uchar* input, __local float3 rgb[LOCAL_IMAGE_WIDTH][LOCAL_IMAGE_HEIGHT], const int width, const int height)
{
    int flatLocal = get_local_id(1) * WORK_GROUP_X + get_local_id(0);

    if (flatLocal < PIXEL_CACHE_JOBS)
    {
        const int2 coord = (int2)(flatLocal % LOCAL_IMAGE_WIDTH, flatLocal / LOCAL_IMAGE_WIDTH);
        const int2 final_coord = gid + coord - (int2)(HALF_FILTER_SIZE);
        final_coord.x = clamp(final_coord.x, 0, width - 1);
        
        for (int k = 0; k < LOCAL_IMAGE_HEIGHT; k += PIXEL_CACHE_STEP)
        {
            const int y = clamp(final_coord.y + k, 0, height - 1);
            
            const int idx1 = y * width + final_coord.x;
            
            rgb[coord.x][coord.y + k] = convert_float3(vload3(idx1, input));
        }
    }
  
    barrier(CLK_LOCAL_MEM_FENCE);
}

__attribute__((reqd_work_group_size(WORK_GROUP_X, WORK_GROUP_Y, 1)))
__kernel void blur_filter_fast(__global uchar* input, __global uchar* output, const int width, const int height)
{
    const int2 gid = (int2)(get_group_id(0) * get_local_size(0), get_group_id(1) * get_local_size(1));
    const int2 lid = (int2)(get_local_id(0), get_local_id(1));

    __local float3 local_image[LOCAL_IMAGE_WIDTH][LOCAL_IMAGE_HEIGHT];
    
    prefetch_pixels(gid, lid, input, local_image, width, height);
    
    const int2 pos = gid + lid;
    const float3 result = pixel_average(lid, local_image);
    
    vstore3(convert_uchar3(result), pos.y * width + pos.x, output);
}

__kernel void sharp_filter_fast(__global uchar* orig_img, __global uchar* blur_img, __global uchar* output, const int width, const int height)
{    
    const int2 gid = (int2)(get_group_id(0) * get_local_size(0), get_group_id(1) * get_local_size(1));
    const int2 lid = (int2)(get_local_id(0), get_local_id(1));
    const int2 pos = gid + lid;
    const int idx = pos.y * width + pos.x;

    __local float3 local_image[LOCAL_IMAGE_WIDTH][LOCAL_IMAGE_HEIGHT];
    
    prefetch_pixels(gid, lid, blur_img, local_image, width, height);
      
    const float3 blur_clr = pixel_average(lid, local_image);
    const float3 orig_clr = convert_float3(vload3(idx, orig_img));
    
    uchar3 result = convert_uchar3(orig_clr * (float3)(ALPHA) + blur_clr * (float3)(BETA) + (float3)(GAMMA));
    result = clamp(result, (uchar3)(0), (uchar3)(UCHAR_MAX));
    
    vstore3(result, idx, output);
}
