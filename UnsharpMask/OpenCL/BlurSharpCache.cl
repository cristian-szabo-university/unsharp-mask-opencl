#define PIXEL_WORK_ITEMS 4

#define LOCAL_IMAGE_WIDTH ((WORK_GROUP_X) + (HALF_FILTER_SIZE) * 2)
#define LOCAL_IMAGE_HEIGHT ((WORK_GROUP_Y) + (HALF_FILTER_SIZE) * 2)
#define LOCAL_IMAGE_SIZE (LOCAL_IMAGE_WIDTH * LOCAL_IMAGE_HEIGHT)

#define PIXEL_CACHE_STEP ((LOCAL_IMAGE_HEIGHT) / (PIXEL_WORK_ITEMS))
#define PIXEL_CACHE_JOBS (LOCAL_IMAGE_SIZE / (PIXEL_WORK_ITEMS))

const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

inline float4 pixel_average(int2 lid, __local float4 rgb[LOCAL_IMAGE_WIDTH][LOCAL_IMAGE_HEIGHT])
{
    float4 sum = (float4)(0.0f);

    for (int y = 0; y < FILTER_SIZE; ++y)
	{
		for (int x = 0; x < FILTER_SIZE; ++x)
		{
            sum += rgb[lid.x + x][lid.y + y];
        }
    }
    
    return (sum / (float4)(FILTER_AREA));
}

inline void prefetch_pixels(int2 gid, int2 lid, __read_only image2d_t input, __local float4 rgb[LOCAL_IMAGE_WIDTH][LOCAL_IMAGE_HEIGHT])
{
    int flatLocal = get_local_id(1) * WORK_GROUP_X + get_local_id(0);

    if (flatLocal < PIXEL_CACHE_JOBS)
    {
        const int2 coord = (int2)(flatLocal % LOCAL_IMAGE_WIDTH, flatLocal / LOCAL_IMAGE_WIDTH);
        const int2 final_coord = gid + coord - (int2)(HALF_FILTER_SIZE);

        for (int k = 0; k < LOCAL_IMAGE_HEIGHT; k += PIXEL_CACHE_STEP)
        { 
            rgb[coord.x][coord.y + k] = read_imagef(input, smp, final_coord + (int2)(0, k));
        }
    }
  
    barrier(CLK_LOCAL_MEM_FENCE);
}

__attribute__((reqd_work_group_size(WORK_GROUP_X, WORK_GROUP_Y, 1)))
__kernel void blur_filter(__read_only image2d_t input, __write_only image2d_t output)
{
    const int2 gid = (int2)(get_group_id(0) * get_local_size(0), get_group_id(1) * get_local_size(1));
    const int2 lid = (int2)(get_local_id(0), get_local_id(1));
    const int2 pos = gid + lid;
    
    __local float4 local_image[LOCAL_IMAGE_WIDTH][LOCAL_IMAGE_HEIGHT];
    
    prefetch_pixels(gid, lid, input, local_image);
    
    const float4 result = pixel_average(lid, local_image);
    
    write_imagef(output, pos, result);
}

__attribute__((reqd_work_group_size(WORK_GROUP_X, WORK_GROUP_Y, 1)))
__kernel void sharp_filter(__read_only image2d_t orig_img, __read_only image2d_t blur_img, __write_only image2d_t output)
{
    const int2 gid = (int2)(get_group_id(0) * get_local_size(0), get_group_id(1) * get_local_size(1));
    const int2 lid = (int2)(get_local_id(0), get_local_id(1));
    const int2 pos = gid + lid;
    
    __local float4 local_image[LOCAL_IMAGE_WIDTH][LOCAL_IMAGE_HEIGHT];
    
    prefetch_pixels(gid, lid, blur_img, local_image);
    
    const float4 orig_clr = read_imagef(orig_img, smp, pos);
    const float4 blur_clr = pixel_average(lid, local_image);
    
    float4 result = orig_clr * (float4)(ALPHA) + blur_clr * (float4)(BETA) + (float4)(GAMMA);
    result = clamp(result, (float4)(0.0f), (float4)(1.0f));
    
    write_imagef(output, pos, result);
}
