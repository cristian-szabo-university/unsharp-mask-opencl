__kernel void gaussian_sharp_filter(image2d_t input, __constant float* filter, image2d_t output)
{
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;
    
    const int2 coord = (int2)(get_global_id(0), get_global_id(1));
    
    size_t index = 0;
    float4 blur = (float4)(0.0f);
    
    for (int y = -HALF_FILTER_SIZE; y <= HALF_FILTER_SIZE; y++)
	{
		for (int x = -HALF_FILTER_SIZE; x <= HALF_FILTER_SIZE; x++, index++)
		{
            blur += read_imagef(input, smp, coord + (int2)(x, y)) * filter[index];
        }
    }
    
    const float4 clr = read_imagef(input, smp, coord);
    
    const float4 result = clr * (float4)(ALPHA) + blur * (float4)(BETA) + (float4)(GAMMA);
    
    write_imagef(output, coord, result);
}