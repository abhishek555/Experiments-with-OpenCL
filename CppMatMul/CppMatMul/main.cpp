#include<iostream>
#include"cll.h"
#include"util.h"
#include<stdio.h>
int main()
{
	std::cout<<"hello OpenCL\n";
	CL example;
	example.loadData();
	#include"matmul.cl"
	example.loadProgram(kernel_source);
	example.createKernel("matmul");
	example.runKernel();

}