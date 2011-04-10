#ifndef CLL_H
#define CLL_H
#include "CL/cl.hpp"
#include<vector>
#include<string>

class CL
{
private:
	std::vector<cl::Platform> platforms;
	std::vector<cl::Device> devices;
	cl::Buffer filterhor,filterver;
	cl::Kernel kernel;
	cl::CommandQueue queue;
	cl::Context context;
	cl::Program program;
	cl_int err;
	cl::Image2D inp,out;
	unsigned char header [54];
public:
	CL();
	~CL();
	void loadData(char[],int[],int[]);
	void loadProgram(std::string);
	void createKernel(const std::string);
	void runKernel();
	void createImage2D(char[]);
};

#endif