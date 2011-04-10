#define STRINGIFY(A) #A

std::string kernel_source= STRINGIFY(
__kernel void Sobel(__read_only image2d_t img1,__write_only image2d_t img2,__global int* filterhor,__global int* filterver)
{
	const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                      CLK_ADDRESS_CLAMP | //Clamp to zeros
                      CLK_FILTER_NEAREST; //Don't interpolate
    int2 coord = (int2)(get_global_id(0), get_global_id(1));
	uint4 val = read_imageui(img1, smp, coord);
	if(coord.x>1&&coord.y>1&&coord.x<511&&coord.y<511)
	{
		
		uint4 val1=read_imageui(img1, smp, (int2)(coord.x-1,coord.y-1));
		uint4 val2=read_imageui(img1, smp, (int2)(coord.x,coord.y-1));
		uint4 val3=read_imageui(img1, smp, (int2)(coord.x+1,coord.y-1));
		uint4 val4=read_imageui(img1, smp, (int2)(coord.x-1,coord.y));
		uint4 val5=read_imageui(img1, smp, (int2)(coord.x,coord.y));
		uint4 val6=read_imageui(img1, smp, (int2)(coord.x+1,coord.y));
		uint4 val7=read_imageui(img1, smp, (int2)(coord.x-1,coord.y+1));
		uint4 val8=read_imageui(img1, smp, (int2)(coord.x,coord.y+1));
		uint4 val9=read_imageui(img1, smp, (int2)(coord.x+1,coord.y+1));
		/*int x1=-val1.x-2*val2.x-val3.x+val7.x+2*val8.x+val9.x;
		int y1=-val1.y-2*val2.y-val3.y+val7.y+2*val8.y+val9.y;
		int z1=-val1.z-2*val2.z-val3.z+val7.z+2*val8.z+val9.z;
		
		int x2=-val1.x-2*val4.x-val7.x+val3.x+2*val6.x+val9.x;
		int y2=-val1.y-2*val4.y-val7.y+val3.y+2*val6.y+val9.y;
		int z2=-val1.z-2*val4.z-val7.z+val3.z+2*val6.z+val9.z;
		*/
		int x1=filterhor[0]*val1.x+filterhor[1]*val2.x+filterhor[2]*val3.x+filterhor[3]*val4.x+filterhor[4]*val5.x+filterhor[5]*val6.x+filterhor[6]*val7.x+filterhor[7]*val8.x+filterhor[8]*val9.x;
		int y1=filterhor[0]*val1.y+filterhor[1]*val2.y+filterhor[2]*val3.y+filterhor[3]*val4.y+filterhor[4]*val5.y+filterhor[5]*val6.y+filterhor[6]*val7.y+filterhor[7]*val8.y+filterhor[8]*val9.y;
		int z1=filterhor[0]*val1.z+filterhor[1]*val2.z+filterhor[2]*val3.z+filterhor[3]*val4.z+filterhor[4]*val5.z+filterhor[5]*val6.z+filterhor[6]*val7.z+filterhor[7]*val8.z+filterhor[8]*val9.z;
		int x2=filterver[0]*val1.x+filterver[1]*val2.x+filterver[2]*val3.x+filterver[3]*val4.x+filterver[4]*val5.x+filterver[5]*val6.x+filterver[6]*val7.x+filterver[7]*val8.x+filterver[8]*val9.x;
		int y2=filterver[0]*val1.y+filterver[1]*val2.y+filterver[2]*val3.y+filterver[3]*val4.y+filterver[4]*val5.y+filterver[5]*val6.y+filterver[6]*val7.y+filterver[7]*val8.y+filterver[8]*val9.y;
		int z2=filterver[0]*val1.z+filterver[1]*val2.z+filterver[2]*val3.z+filterver[3]*val4.z+filterver[4]*val5.z+filterver[5]*val6.z+filterver[6]*val7.z+filterver[7]*val8.z+filterver[8]*val9.z;
		val.x=sqrt((float)(x1*x1+x2*x2));
		val.y=sqrt((float)(y1*y1+y2*y2));
		val.z=sqrt((float)(z1*z1+z2*z2));
				
	}
	write_imageui(img2, coord, val);
	
}
);