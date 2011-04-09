#include <CL/cl.h>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>

#define SIDE 20
#define GROUP_SIZE 20
#define KERNEL "mmmLocal"
#define KERNEL_FILE "oclMatAdd.cl"

/* Host data */
cl_uint widthA = SIDE;    //widthA = heightB
cl_uint heightB = widthA;
cl_uint heightA = SIDE;    //heightA = heightC
cl_uint heightC = heightA;
cl_uint widthB = SIDE;    //widthB = widthC
cl_uint widthC = widthB;

cl_uint lengthA = widthA * heightA;
cl_uint lengthB = widthB * heightB;
cl_uint lengthC = widthC * heightC;

cl_float* inputA = NULL;
cl_float* inputB = NULL;
cl_float* output = NULL;
cl_float temp;
/* OCL parameters */
cl_platform_id platform;
cl_context context;
cl_command_queue commandQueue;
cl_device_id device;
cl_program program;
cl_kernel kernel;
cl_device_type dType = CL_DEVICE_TYPE_CPU;

cl_event event_;

/* OCL memory buffers */
cl_mem inputBufferA;
cl_mem inputBufferB;
cl_mem outputBuffer;

/* Host buffer for reference computation */
cl_float *referenceOutput;

/*
   Saxpy CPU reference
*/

double diffclock(clock_t clock1,clock_t clock2)
{
	double diffticks=clock1-clock2;
	double diffms=(diffticks*1000)/CLOCKS_PER_SEC;
	return diffms;
} 



void sumCPU()
{
clock_t begin=clock();

    referenceOutput = (cl_float*)malloc(sizeof(cl_float) * lengthC);
    memset(referenceOutput, 0, lengthC * sizeof(cl_float));

    //For each element of C
    for(cl_uint i=0; i < heightC; i++)
    {
        for(cl_uint j=0; j < widthC; j++)
        {
            //Compute from corresponding row of A and column of B
                      
                referenceOutput[i*widthC + j] = (inputA[i*widthA + j]+inputB[i*widthB + j]);
				//referenceOutput[i*widthC + j]-= (inputA[i*widthA + j]*inputB[i*widthB + j]);
				//referenceOutput[i*widthC + j]+= (inputA[i*widthA + j]/inputB[i*widthB + j]);
				
        }
    }

    clock_t end=clock();
    std::cout << "Time elapsed:<inside sumCPU> " << double(diffclock(end,begin)) << " ms"<< std::endl;
	
}

void setupSum()
{
    inputA = (cl_float*)malloc(lengthA * sizeof(cl_float));
    inputB = (cl_float*)malloc(lengthB * sizeof(cl_float));
    output = (cl_float*)malloc(lengthC * sizeof(cl_float));
	memset(output, 0, lengthC * sizeof(cl_float));

    for(int i = 0; i < (int)lengthA; i++)
    {
        /* Assign random values to input Data */
		inputA[i] =(float)(i*10)+1.3;//(2.0f * temp/RAND_MAX - 1.0f);
    }
     for(int i = 0; i < (int)lengthB; i++)
    {
        /* Assign random values to input Data */
        inputB[i] = (float)(i*10)+1.3;//(2.0f * temp/ (float) RAND_MAX - 1.0f);
    }
}

void setupCL()
{
    cl_int status = 0;

    /*
     * Have a look at the available platforms and pick either
     * the AMD one if available or a reasonable default.
     */

    cl_uint numPlatforms;
    status = clGetPlatformIDs(0, NULL, &numPlatforms);

    if (0 < numPlatforms) 
    {
        cl_platform_id* platforms = new cl_platform_id[numPlatforms];
        status = clGetPlatformIDs(numPlatforms, platforms, NULL);

        for (unsigned i = 0; i < numPlatforms; ++i) 
        {
            char pbuf[100];
            status = clGetPlatformInfo(platforms[i],
                                       CL_PLATFORM_VENDOR,
                                       sizeof(pbuf),
                                       pbuf,
                                       NULL);
            platform = platforms[i];
            if (!strcmp(pbuf, "Advanced Micro Devices, Inc.")) 
            {
                break;
            }
        }
        delete[] platforms;
    }
//Get the devices
    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
   
    if (status != CL_SUCCESS)
    {
        printf("\n Error in clGetdevice ID = %d", status);
    }
	/*
	cl_int clGetDeviceInfo (cl_device_id device,
								cl_device_info param_name,
								size_t param_value_size,
								void *param_value,
								size_t *param_value_size_ret)
	*/

	cl_command_queue_properties pp;
	status = clGetDeviceInfo (device, CL_DEVICE_QUEUE_PROPERTIES, sizeof(pp), &pp, NULL);


    /*
     * If we could find our platform, use it. Otherwise pass a NULL 
     * and get whatever the
     * implementation thinks we should be using.
     */

    cl_context_properties cps[3] = 
    {
        CL_CONTEXT_PLATFORM, 
        (cl_context_properties)platform, 
        0
    };

    /* Use NULL for backward compatibility */
    cl_context_properties* cprops = (NULL == platform) ? NULL : cps;

    context = clCreateContext(0, 1, &device, NULL, NULL, &status);
   // context = clCreateContextFromType(cprops, dType, NULL, NULL, &status);

    /* Now, get the device list data */
    status = clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(device), &device, NULL);

    /* create Command queue */
    commandQueue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &status);

    /* Create OCL buffers */
    inputBufferA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_float) * lengthA, inputA, &status);
    inputBufferB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_float) * lengthB, inputB, &status);
    outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_float) * lengthC, 0, &status);

    /* Read kernel file to a string */
    size_t size;
    char* str;

    // Open file stream
    std::fstream f(KERNEL_FILE, (std::fstream::in | std::fstream::binary));

    // Check if we have opened file stream
    if (f.is_open()) 
    {
        size_t  sizeFile;
        // Find the stream size
        f.seekg(0, std::fstream::end);
        size = sizeFile = f.tellg();
        f.seekg(0, std::fstream::beg);

        str = new char[size + 1];
        if (!str) 
        {
            f.close();
            return;
        }

        // Read file
        f.read(str, sizeFile);
        f.close();
        str[size] = '\0';
    }
    else
    {
        return;
    }

    /* Create program object from source */
    program = clCreateProgramWithSource(context, 1, (const char **)&str, &size, &status);

    /* create a cl program executable for all the devices specified */
    status = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if(status != CL_SUCCESS)
    {
        if(status == CL_BUILD_PROGRAM_FAILURE)
        {
            cl_int logStatus;
            char * buildLog = NULL;
            size_t buildLogSize = 0;
            logStatus = clGetProgramBuildInfo (program, device, CL_PROGRAM_BUILD_LOG, buildLogSize, buildLog, &buildLogSize);

            buildLog = (char*)malloc(buildLogSize);
            memset(buildLog, 0, buildLogSize);

            logStatus = clGetProgramBuildInfo (program, device, CL_PROGRAM_BUILD_LOG, buildLogSize, buildLog, NULL);
 
            std::cout << " \n\t\t\tBUILD LOG\n";
            std::cout << " ************************************************\n";
            std::cout << buildLog << std::endl;
            std::cout << " ************************************************\n";
            free(buildLog);
        }
    }

    /* get a kernel object handle for a kernel with the given name */
    kernel = clCreateKernel(program, KERNEL, &status);
	if(status != CL_SUCCESS)
	{
		printf(" Error : clCreateKErnel failed! \n");
		return;
	}

	size_t kernelWorkGroupSize;
	status = clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &kernelWorkGroupSize, 0);

}

void runKernel()
{
    cl_int status;
	int block_size = GROUP_SIZE;

    /* Set up arguments to the kernel */
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&inputBufferA);
	if(status != CL_SUCCESS)
	{
		printf(" Error : clSetKernelArg 0 failed!  Error Code = %d\n", status);
		return;
	}

    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &inputBufferB);
	if(status != CL_SUCCESS)
	{
		printf(" Error : clSetKernelArg 1 failed!  Error Code = %d\n", status);
		return;
	}

	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &outputBuffer);
	if(status != CL_SUCCESS)
	{
		printf(" Error : clSetKernelArg 2 failed!  Error Code = %d\n", status);
		return;
	}

    
	status = clSetKernelArg(kernel, 3, sizeof(cl_int), &lengthC);
	if(status != CL_SUCCESS)
	{
		printf(" Error : clSetKernelArg 2 failed!  Error Code = %d\n", status);
		return;
	}

	const size_t globalSize[2] = {lengthC, lengthC};
	const size_t localSize[2] = {GROUP_SIZE,GROUP_SIZE};

	cl_event event_1;
	status = clFinish(commandQueue);
	//clEnqueueWriteBuffer(commandQueue,inputBufferA, CL_FALSE, 0, sizeof(cl_float)*lengthA, inputA, 0, NULL, NULL);
	//clEnqueueWriteBuffer(commandQueue,inputBufferB, CL_FALSE, 0, sizeof(cl_float)*lengthB, inputB, 0, NULL, NULL);
	if(status!=CL_SUCCESS)
		printf("\nError");

	//status = clFinish(commandQueue);
	clock_t begin=clock();
	status = clEnqueueNDRangeKernel(commandQueue, kernel, 1, 0, globalSize, localSize, 0, 0, &event_1);
	
	if(status!=CL_SUCCESS)
		printf("\nError--%d",status);
	status = clFinish(commandQueue);
	//status = clFinish(commandQueue);
	status = clEnqueueReadBuffer(commandQueue, outputBuffer, 1, 0, lengthC * sizeof(cl_float), output, 0, 0, &event_);
	if(status!=CL_SUCCESS)
		printf("\nError\n");

	clock_t end=clock();
	std::cout << "Time elapsed:<call clEnque> " << double(diffclock(end,begin)) << " ms"<< std::endl;
}

int main()
{
    {
		clock_t begin=clock();
		setupSum();
		clock_t end=clock();
		std::cout << "Time elapsed:<call setupSum> " << double(diffclock(end,begin)) << " ms"<< std::endl;
    }


    {
		clock_t begin=clock();
		setupCL();
		clock_t end=clock();
		std::cout << "Time elapsed:<call setupCL> " << double(diffclock(end,begin)) << " ms"<< std::endl;
    }


    {
		clock_t begin=clock();
		runKernel();
		clock_t end=clock();
		std::cout << "Time elapsed:<call runKernal> " << double(diffclock(end,begin)) << " ms"<< std::endl;
    }

    {
		clock_t begin=clock();
		/* Compute reference FFT */
		sumCPU();
		clock_t end=clock();
		std::cout << "Time elapsed:<call sumCPU> " << double(diffclock(end,begin)) << " ms"<< std::endl;
    }


    {
		/* Compare results */
		clock_t begin=clock();
		float error = 0;
		for(int i = 0; i < (int)lengthC; i++)
		{
			std::cout<<"input ="<<inputA[i]<<"\tref output="<<referenceOutput[i]<<"\t output= "<<output[i]<<"\n";
			error += abs((float)(referenceOutput[i] - output[i]));
		}   
		clock_t end=clock();
		std::cout << "Time elapsed:<camparing result> " << double(diffclock(end,begin)) << " ms"<< std::endl;

		if(error < 1)
	        std::cout << "Passed! " << std::endl;
		else
	        std::cout << "Failed! " << std::endl;
    }
    /* Free host resources */
    free(inputA);
    free(inputB);
    free(output);
	getchar();
	getchar();
}