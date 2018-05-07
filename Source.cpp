#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "MakoFrameGrab.h"

using namespace cv;
using namespace std;


int main()
{
	Mat frame;

	MakoFrameGrab camera;

	camera.run();

	for (int i = 0; i < 100; i++)
	{
		camera.grabFrame(frame);

		imshow("frame", frame);

		waitKey(20);
	}

	camera.end();

	return 0;
}
