# define STRINGIFY(A) #A

std::string kernel_source=STRINGIFY(

__kernel void matmul(__global float*a,__global float*b,__global float*c,int awidth,int bwidth,int cwidth)
{
	int row=get_global_id(0);
	int col=get_global_id(1);
	float ctemp=0;
	for(int i=0;i<awidth;++i)
		ctemp+=a[row*awidth+i]*b[i*bwidth+col];
	c[row*cwidth+col]=ctemp;
}
);