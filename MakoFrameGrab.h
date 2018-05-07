#pragma  once
#include <cstdio>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <PvApi.h>

using namespace cv;
using namespace std;

//Global struct to store info on the Mako camera connected

class MakoFrameGrab {

public:

	void run();

	void grabFrame(Mat & singleFrame);

	void end();

private:

	typedef struct
	{
		unsigned long   UID;
		tPvHandle       Handle;
		tPvFrame        Frame;
	} tCamera;


	tCamera		myCamera;
	tPvErr		Errcode;
};