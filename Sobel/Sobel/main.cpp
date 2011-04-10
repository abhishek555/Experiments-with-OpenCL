#include "cll.h"
int main()
{
	int filterhor[9]={-1,-2,-1,0,0,0,1,2,1},filterver[9]={-1,0,1,-2,0,2,-1,0,1};
	//int filterhor[9]={0,1,0,1,-4,1,0,1,0},filterver[9]={0,0,0,0,0,0,0,0,0};
	CL example;
	example.loadData("C://lena1.bmp",filterhor,filterver);
	#include"Sobel.cl"
	example.loadProgram(kernel_source);
	example.createKernel("Sobel");
	example.runKernel();
}