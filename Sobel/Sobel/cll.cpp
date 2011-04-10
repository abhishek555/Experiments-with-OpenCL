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


void CL::loadData(char name[],int* filterh,int* filterv)
{
	
	cout<<"Loading Data\n";
	clock_t begin=clock();
	createImage2D(name);
	filterhor=cl::Buffer::Buffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,9*sizeof(int),filterh,&err);
	if(err!=CL_SUCCESS)
		cout<<"allocation of filter horizontal";
	filterver=cl::Buffer::Buffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,9*sizeof(int),filterv,&err);
	if(err!=CL_SUCCESS)
		cout<<"allocation of filter vertical";
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

void CL::createKernel(const string kernel_name)
{
	cout<<"Create Kernel\n";
	clock_t begin=clock();
	kernel=cl::Kernel(program,kernel_name.data(),&err);
	if(err!=CL_SUCCESS)
		cout<<err<<endl;
	err=kernel.setArg(0,inp);
	err=kernel.setArg(1,out);
	err=kernel.setArg(2,filterhor);
	err=kernel.setArg(3,filterver);
	if(err!=CL_SUCCESS)
		cout<<"SetKernel Arg\n"<<err;
	clock_t end=clock();
	cout<<"Time Create Kernel "<<diffclock(end,begin)<<endl;;

}

void CL::runKernel()
{
	cout<<"Running Kernel\n";
	clock_t begin=clock();
	queue.enqueueNDRangeKernel(kernel,cl::NullRange,cl::NDRange(512,512),cl::NullRange);
	queue.finish();
	FILE *output = fopen("D://lena.bmp", "wb");
	fwrite(header,1,54,output);
	unsigned char* pixels = new unsigned char[512*512*4];
	cl::size_t<3> origin,region;
	origin[0]=0;
	origin[1]=0;
	origin[2]=0;
	region[0]=512;
	region[1]=512;
	region[2]=1;
	err=queue.enqueueReadImage(out,CL_TRUE,origin,region,0,0,pixels);
	if(err!=CL_SUCCESS)
	{
		cout<<"Error Reading Data\n";
		cout<<err;
	}
	queue.finish();
	fseek (output, 54, SEEK_SET);
	fwrite(pixels, 1,4*512*512,output);
	fclose(output);
	clock_t end=clock();
	cout<<"Time Running Kernel "<<diffclock(end,begin)<<endl;
	
}

void CL::createImage2D(char name[])
{
   size_t result=0;
      
   // dimensions
   size_t width = 512;
   size_t height = 512;
   size_t rowpitch = 0;

   FILE *input = fopen(name, "rb");
   if(!input)
   {
	   cout<<name<<"error file";
	   return;
   }
   // store and skip header
   result = fread(header,1,54,input); 
   fseek (input, 54, SEEK_SET);
   unsigned char* pixels = new unsigned char[4*width*height];
   result = fread( pixels, 1, 4*width*height, input );
   fclose(input);
   cl::ImageFormat format(CL_RGBA,CL_UNSIGNED_INT8);
   cl_mem_flags flags;
   flags = CL_MEM_USE_HOST_PTR;
   inp=cl::Image2D(context,CL_MEM_COPY_HOST_PTR,format,512,512,0,(void*)pixels,&err);
   if(err!=CL_SUCCESS)
	   cout<<"Inp data Transfer\n";
   cl::ImageFormat format1(CL_RGBA,CL_UNSIGNED_INT8);
   out=cl::Image2D(context,CL_MEM_WRITE_ONLY,format1,512,512,0,NULL,&err);
   if(err!=CL_SUCCESS)
	   cout<<"Out data Transfer\n";
   
}