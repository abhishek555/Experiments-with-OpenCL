/* Write MMM using local memory */
__kernel void mmmLocal(__global float* A,__global float* B,__global float* C,int size) 
{	
	int i = get_global_id(0);
	if(i>size)
	return;
	int j = get_global_id(1);
    C[i] = A[i]+B[i];
    
}
