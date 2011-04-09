#ifndef CLL_H
#define CLL_H
#include "CL/cl.hpp"
#include <vector>
#include<string>
#define SIZE 2

class CL
{
private:
	std::vector<cl::Platform> platforms;
	std::vector<cl::Device> devices;
	cl_int err;
	cl::Context context;
	cl::Buffer cl_a,cl_b,cl_c;
	cl::CommandQueue queue;
	cl::Kernel kernel;
	cl::Program program;
	cl::Event evnt;

public:
	CL();
	void showPlatformsAvailable();
	void showDevicesAvailable();
	void loadData();
	void loadProgram(std::string kernel_source);
	void createKernel(std::string kernel_name);
	void runKernel();
	~CL();
};
#endif
