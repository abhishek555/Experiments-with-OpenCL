#include "cll.h"
#include<iostream>
#include<string>
#include"util.h"

using namespace std;
CL::CL()
{
	cout<<"Constructor CL\n";
	clock_t begin=clock();
	err=cl::Platform::get(&platforms);
	if(err!=CL_SUCCESS)
	{
		cout<<"Platform";
		return;
	}
	string platform_name;
	string device_type;
	cout<<"Number of Platforms Available:"<<platforms.size()<<endl;
	platforms[0].getInfo(CL_PLATFORM_NAME,&platform_name);
	cout<<"Platform Used:"+platform_name<<endl;
	err=platforms[0].getDevices(CL_DEVICE_TYPE_ALL,&devices);
	if(err!=CL_SUCCESS)
	{
		cout<<"Device";
		return;
	}
	cout<<"Number of Devices Available:"<<devices.size()<<endl;
	err=devices[0].getInfo(CL_DEVICE_NAME,&device_type);
	if(err!=CL_SUCCESS)
		cout<<"Type of device";
	else
		cout<<"Type of Device Used: "+device_type<<endl;
	context=cl::Context(devices,NULL,NULL,NULL,&err);
	if(err!=CL_SUCCESS)
		cout<<"Context";
	queue=cl::CommandQueue(context,devices[0],NULL,&err);
	if(err!=CL_SUCCESS)
		cout<<"Command Queue";
	clock_t end=clock();
	cout<<"Time Constructor: "<<diffclock(end,begin)<<endl;


}
CL::~CL()
{
}

void CL::showDevicesAvailable()
{
}
void CL::showPlatformsAvailable()
{
}
void CL::loadData()
{
	
	cout<<"Loading Data\n";
	clock_t begin=clock();
	float *a=new float[SIZE*SIZE];
	float *b=new float[SIZE*SIZE];
	float *c=new float[SIZE*SIZE];
	for(int i=0;i<SIZE;++i)
		for(int j=0;j<SIZE;++j)
		{
			a[i*SIZE+j]=(i+j+1)*2.3f;
			b[i*SIZE+j]=(i+j+1)*3.2f;
			c[i*SIZE+j]=0;
		}
	cl_a=cl::Buffer::Buffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,SIZE*SIZE*sizeof(float),a,&err);
	if(err!=CL_SUCCESS)
		cout<<"allocation of a";
	cl_b=cl::Buffer::Buffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,SIZE*SIZE*sizeof(float),b,&err);
	if(err!=CL_SUCCESS)
		cout<<"allocation of b";
	cl_c=cl::Buffer::Buffer(context,CL_MEM_WRITE_ONLY,SIZE*SIZE*sizeof(float),c,&err);
	if(err!=CL_SUCCESS)
		cout<<"allocation of c";
	
	delete a;
	delete b;
	delete c;
	clock_t end=clock();
	cout<<"Time Load Data "<<diffclock(end,begin)<<endl;

}

void CL::loadProgram(string kernel_source)
{
	cout<<"Load Program\n";
	clock_t begin=clock();
	cl::Program::Sources source(1,make_pair(kernel_source.c_str(),kernel_source.size()));
	program=cl::Program(context,source,&err);
	if(err!=CL_SUCCESS)
		cout<<"Program";
	err=program.build(devices);
	if(err!=CL_SUCCESS)
		cout<<"Build Error";
	cout<<"done building program\n";
	clock_t end=clock();
	cout<<"Time Build Program "<<diffclock(end,begin)<<endl;
	cout << "Build Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices[0]) << std::endl;
	cout << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices[0]) << std::endl;
	cout << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << std::endl;
	
}

void CL::createKernel(const string kernel_name )
{
	cout<<"Create Kernel\n";
	clock_t begin=clock();
	kernel=cl::Kernel(program,kernel_name.data(),&err);
	if(err!=CL_SUCCESS)
		cout<<err<<endl;
	err=kernel.setArg(0,cl_a);
	err=kernel.setArg(1,cl_b);
	err=kernel.setArg(2,cl_c);
	err=kernel.setArg(3,SIZE);
	err=kernel.setArg(4,SIZE);
	err=kernel.setArg(5,SIZE);
	clock_t end=clock();
	cout<<"Time Create Kernel "<<diffclock(end,begin)<<endl;;

}

void CL::runKernel()
{
	cout<<"Running Kernel\n";
	clock_t begin=clock();
	queue.enqueueNDRangeKernel(kernel,cl::NullRange,cl::NDRange(SIZE,SIZE),cl::NullRange);
	queue.finish();
	float output[SIZE*SIZE];
	queue.enqueueReadBuffer(cl_c,CL_TRUE,0,SIZE*SIZE*sizeof(float),output);
	queue.finish();
	clock_t end=clock();
	cout<<"Time Running Kernel "<<diffclock(end,begin)<<endl;
	cout<<"Output\n";
	for(int i=0;i<SIZE;++i)
		for(int j=0;j<SIZE;++j)
			cout<<output[i*SIZE+j]<<endl;
}