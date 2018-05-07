#include "MakoFrameGrab.h"


void MakoFrameGrab::run() {
	// Initialize the PvAPI interface so that we can look for cameras
	if (!PvInitialize()) {
		// Wait for the response from a camera after the initialization of the driver
		// This is done by checking if camera's are found yet
		while (PvCameraCount() == 0)
		{
			waitKey(15);
		}

		// If there is a camera connecte to the camera 1 interface, grab it!
		tPvCameraInfo cameraInfo;
		if (PvCameraList(&cameraInfo, 1, NULL) == 1) //(array of PvCameraInfo to return values to, length of array, variable to return number of camera to)
		{
			unsigned long frameSize;

			// Get the camera ID
			myCamera.UID = cameraInfo.UniqueId;
			// Open the camera
			if (!PvCameraOpen(myCamera.UID, ePvAccessMaster, &(myCamera.Handle))) //(CamID, Control Mode, Place to return camera handle)
			{
				// Debug
				cout << "Camera opened succesfully" << endl;

				// Get the image size of every capture
				//PvAttrUint32Get returns an attribute of a camera specified by a specific handle
				PvAttrUint32Get(myCamera.Handle, "TotalBytesPerFrame", &frameSize); //(CamHandle,AttributeName,Place to store attribute value)
				cout << "Framesize: " << frameSize << endl;

				// Allocate a buffer to store the image
				memset(&myCamera.Frame, 0, sizeof(tPvFrame)); //Writes the value 0 to tCamera's "Frame"-variable
				myCamera.Frame.ImageBufferSize = frameSize; //Dont know about the tPvFrame tbh
				myCamera.Frame.ImageBuffer = new char[frameSize];

				// Set the manta camera parameters
				PvAttrUint32Set(myCamera.Handle, "ExposureValue", 20432); //Axel input for exposure time (ms)

																		  // Start the camera
				PvCaptureStart(myCamera.Handle); //Initializes the "data stream" apparently

												 // Set the camera to capture continuously
				PvAttrEnumSet(myCamera.Handle, "AcquisitionMode", "Continuous"); //Sets acq. mode obviously
				PvCommandRun(myCamera.Handle, "AcquisitionStart"); //Readies for a trigger to capture frame
				PvAttrEnumSet(myCamera.Handle, "FrameStartTriggerMode", "Freerun"); //Determine what should trigger the frame capturing, not sure what freerun is

			}
			else {
				cout << "Opening camera error" << endl;
			}
		}
		else {
			cout << "Camera not found or opened unsuccesfully" << endl;
		}
	}
	else {
		// State that we did not succeed in initializing the API
		cout << "Failed to initialise the camera API" << endl;
	}
}

void MakoFrameGrab::grabFrame(Mat& singleFrame)
{
	//Temp image
	Mat image;

	//I believe this is the trigger of "freerun" for capturing image buffer
	if (!PvCaptureQueueFrame(myCamera.Handle, &(myCamera.Frame), NULL))//Places an image buffer to a frame (cam handle, pointer to frame to return to, pointer to return errortype)
	{
		while (PvCaptureWaitForFrameDone(myCamera.Handle, &(myCamera.Frame), 100) == ePvErrTimeout)//blocks until frame is completed (cam_handle,pointer to frame,time_out in ms) returns error type.
		{
		}

		// Create an image header (mono image)
		// Push ImageBuffer data into the image matrix
		image = Mat(myCamera.Frame.Height, myCamera.Frame.Width, CV_8U);
		image.data = (uchar *)myCamera.Frame.ImageBuffer; //ImageBuffer is the content of the frame

														  // Convert from Bayer Pattern (single channel) to BGR colour image
		Mat color = Mat(myCamera.Frame.Height, myCamera.Frame.Width, CV_8UC3);
		cvtColor(image, singleFrame, CV_BayerBG2BGR);

		// Release the image data (unnecessary when only grabbing 1 frame at a time)
		//image.release();
	}
}

void MakoFrameGrab::end()
{
	Errcode = PvCommandRun(myCamera.Handle, "AcquisitionStop");
	if (Errcode != ePvErrSuccess)
		throw Errcode;

	PvCaptureEnd(myCamera.Handle);
	PvCameraClose(myCamera.Handle);
}