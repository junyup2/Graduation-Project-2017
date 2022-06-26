#include <cv.h>
#include "stdafx.h"
#include "opencv2/opencv.hpp"
#include "BlobLabeling.h"
#include <math.h>
#include <stdio.h>
#include <conio.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/tracking.hpp"

#include <iostream>
#include <fstream>
#include <string>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <Windows.h>
#include <time.h>

using namespace cv;
using namespace std;

#define SCROLLUP 20
#define SCROLLDOWN -20

#define ID_HEURES        2003



void gotoxy(int x, int y)      // ÁÂÇ¥ º¸³»±â gotoxy
{
     COORD Cur;
     Cur.X=x;
     Cur.Y=y;
     SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),Cur);
}

void Image_Conv(IplImage* Src_RImage, IplImage* Gray_RImage, IplImage* TH_RImage)
{
	cvFlip(Src_RImage,Src_RImage,1);
	cvCvtColor( Src_RImage, Gray_RImage, CV_BGR2GRAY );
	cvThreshold(Gray_RImage,TH_RImage,240,255,CV_THRESH_BINARY);
}
  
int main( )
{ 
	//Cam Source//
	IplImage* image = 0; 
	CvCapture* capture = cvCaptureFromCAM(0); 

	Point ptc_RIndex = 0;
	Point ptc_RMiddle = 0;
	Point ptc_RThumb = 0;

	Point ptc_LIndex = 0;
	Point ptc_LMiddle = 0;
	Point ptc_LThumb = 0;

	CvPoint LT_old = {0,0};
	CvPoint LI_old = {0,0};
	CvPoint LM_old = {0,0};

	CvPoint RT_old = {0,0};
	CvPoint RI_old = {0,0};
	CvPoint RM_old = {0,0};

	int count = 0;


	/////Open Data Text/////

	//////////////
	if( capture )
    {
		for(;;)
        {
			cvGrabFrame( capture ); 
			image = cvRetrieveFrame( capture );

			IplImage* Gray_RImage = cvCreateImage( cvSize(image->width,image->height), 8, 1 );
			IplImage* TH_RImage = cvCreateImage( cvSize(image->width,image->height), 8, 1 );
			
			Image_Conv(image,Gray_RImage,TH_RImage);

			CBlobLabeling blobs;
			blobs.SetParam(TH_RImage,0.1);
			blobs.DoLabeling(); 


			for( int i=0; i < blobs.m_nBlobs; i++ )
			{	
				CvPoint pt1 = cvPoint( blobs.m_recBlobs[i].x,blobs.m_recBlobs[i].y );
				CvPoint pt2 = cvPoint( pt1.x + blobs.m_recBlobs[i].width, pt1.y + blobs.m_recBlobs[i].height );
				CvPoint ptc = cvPoint( (pt1.x + blobs.m_recBlobs[i].width/2), (pt1.y + blobs.m_recBlobs[i].height/2));


				if(i>=4)
				{
					CvPoint pt_00 = cvPoint( blobs.m_recBlobs[0].x,blobs.m_recBlobs[0].y );
					CvPoint ptc_00 = cvPoint( (pt_00.x + blobs.m_recBlobs[0].width/2), (pt_00.y + blobs.m_recBlobs[0].height/2));

					CvPoint pt_01 = cvPoint( blobs.m_recBlobs[1].x,blobs.m_recBlobs[1].y );
					CvPoint ptc_01 = cvPoint( (pt_01.x + blobs.m_recBlobs[1].width/2), (pt_01.y + blobs.m_recBlobs[1].height/2));

					CvPoint pt_02 = cvPoint( blobs.m_recBlobs[2].x,blobs.m_recBlobs[2].y );
					CvPoint ptc_02 = cvPoint( (pt_02.x + blobs.m_recBlobs[2].width/2), (pt_02.y + blobs.m_recBlobs[2].height/2));

					CvPoint pt_03 = cvPoint( blobs.m_recBlobs[3].x,blobs.m_recBlobs[3].y );
					CvPoint ptc_03 = cvPoint( (pt_03.x + blobs.m_recBlobs[3].width/2), (pt_03.y + blobs.m_recBlobs[3].height/2));

					CvPoint pt_04 = cvPoint( blobs.m_recBlobs[4].x,blobs.m_recBlobs[4].y );
					CvPoint ptc_04 = cvPoint( (pt_04.x + blobs.m_recBlobs[4].width/2), (pt_04.y + blobs.m_recBlobs[4].height/2));

					CvPoint pt_05 = cvPoint( blobs.m_recBlobs[5].x,blobs.m_recBlobs[5].y );
					CvPoint ptc_05 = cvPoint( (pt_05.x + blobs.m_recBlobs[5].width/2), (pt_05.y + blobs.m_recBlobs[5].height/2));

				//Kalman Filter//	
				//R//
				KalmanFilter KF_RT(4, 2, 0);
				KalmanFilter KF_RI(4, 2, 0);
				KalmanFilter KF_RM(4, 2, 0);
				//L//
				KalmanFilter KF_LT(4, 2, 0);
				KalmanFilter KF_LI(4, 2, 0);
				KalmanFilter KF_LM(4, 2, 0);
		
				// intialization of KF...
				KF_RT.transitionMatrix = *(Mat_<float>(4, 4) << 1,0,1,0,   0,1,0,1,  0,0,1,0,  0,0,0,1);
				Mat_<float> measurement_RT(2,1); measurement_RT.setTo(Scalar(0));

				KF_RT.statePre.at<float>(0) = ptc_02.x;
				KF_RT.statePre.at<float>(1) = ptc_02.y;
				KF_RT.statePre.at<float>(2) = 0;
				KF_RT.statePre.at<float>(3) = 0;

				setIdentity(KF_RT.measurementMatrix);
				setIdentity(KF_RT.processNoiseCov, Scalar::all(1e-4));
				setIdentity(KF_RT.measurementNoiseCov, Scalar::all(10));
				setIdentity(KF_RT.errorCovPost, Scalar::all(.1));
				//////////////////////////////////////////////////////////////////////////////////////////////
				KF_RI.transitionMatrix = *(Mat_<float>(4, 4) << 1,0,1,0,   0,1,0,1,  0,0,1,0,  0,0,0,1);
				Mat_<float> measurement_RI(2,1); measurement_RI.setTo(Scalar(0));

				KF_RI.statePre.at<float>(0) = ptc_00.x;
				KF_RI.statePre.at<float>(1) = ptc_00.y;
				KF_RI.statePre.at<float>(2) = 0;
				KF_RI.statePre.at<float>(3) = 0;

				setIdentity(KF_RI.measurementMatrix);
				setIdentity(KF_RI.processNoiseCov, Scalar::all(1e-4));
				setIdentity(KF_RI.measurementNoiseCov, Scalar::all(10));
				setIdentity(KF_RI.errorCovPost, Scalar::all(.1));
				//////////////////////////////////////////////////////////////////////////////////////////////
				KF_RM.transitionMatrix = *(Mat_<float>(4, 4) << 1,0,1,0,   0,1,0,1,  0,0,1,0,  0,0,0,1);
				Mat_<float> measurement_RM(2,1); measurement_RM.setTo(Scalar(0));

				KF_RM.statePre.at<float>(0) = ptc_01.x;
				KF_RM.statePre.at<float>(1) = ptc_01.y;
				KF_RM.statePre.at<float>(2) = 0;
				KF_RM.statePre.at<float>(3) = 0;

				setIdentity(KF_RM.measurementMatrix);
				setIdentity(KF_RM.processNoiseCov, Scalar::all(1e-4));
				setIdentity(KF_RM.measurementNoiseCov, Scalar::all(10));
				setIdentity(KF_RM.errorCovPost, Scalar::all(.1));
				//////////////////////////////////////////////////////////////////////////////////////////////

				//////////////////////////////////////////////////////////////////////////////////////////////
				KF_LT.transitionMatrix = *(Mat_<float>(4, 4) << 1,0,1,0,   0,1,0,1,  0,0,1,0,  0,0,0,1);
				Mat_<float> measurement_LT(2,1); measurement_LT.setTo(Scalar(0));

				KF_LT.statePre.at<float>(0) = ptc_05.x;
				KF_LT.statePre.at<float>(1) = ptc_05.y;
				KF_LT.statePre.at<float>(2) = 0;
				KF_LT.statePre.at<float>(3) = 0;

				setIdentity(KF_LT.measurementMatrix);
				setIdentity(KF_LT.processNoiseCov, Scalar::all(1e-4));
				setIdentity(KF_LT.measurementNoiseCov, Scalar::all(10));
				setIdentity(KF_LT.errorCovPost, Scalar::all(.1));
				//////////////////////////////////////////////////////////////////////////////////////////////
				KF_LI.transitionMatrix = *(Mat_<float>(4, 4) << 1,0,1,0,   0,1,0,1,  0,0,1,0,  0,0,0,1);
				Mat_<float> measurement_LI(2,1); measurement_LI.setTo(Scalar(0));

				KF_LI.statePre.at<float>(0) = ptc_03.x;
				KF_LI.statePre.at<float>(1) = ptc_03.y;
				KF_LI.statePre.at<float>(2) = 0;
				KF_LI.statePre.at<float>(3) = 0;

				setIdentity(KF_LI.measurementMatrix);
				setIdentity(KF_LI.processNoiseCov, Scalar::all(1e-4));
				setIdentity(KF_LI.measurementNoiseCov, Scalar::all(10));
				setIdentity(KF_LI.errorCovPost, Scalar::all(.1));
				//////////////////////////////////////////////////////////////////////////////////////////////
				KF_LM.transitionMatrix = *(Mat_<float>(4, 4) << 1,0,1,0,   0,1,0,1,  0,0,1,0,  0,0,0,1);
				Mat_<float> measurement_LM(2,1); measurement_LM.setTo(Scalar(0));

				KF_LM.statePre.at<float>(0) = ptc_04.x;
				KF_LM.statePre.at<float>(1) = ptc_04.y;
				KF_LM.statePre.at<float>(2) = 0;
				KF_LM.statePre.at<float>(3) = 0;

				setIdentity(KF_LM.measurementMatrix);
				setIdentity(KF_LM.processNoiseCov, Scalar::all(1e-4));
				setIdentity(KF_LM.measurementNoiseCov, Scalar::all(10));
				setIdentity(KF_LM.errorCovPost, Scalar::all(.1));


				while(1)
				{
					cvGrabFrame( capture ); 
					image = cvRetrieveFrame( capture );

					IplImage* Gray_RImage = cvCreateImage( cvSize(image->width,image->height), 8, 1 );
					IplImage* TH_RImage = cvCreateImage( cvSize(image->width,image->height), 8, 1 );

					Image_Conv(image,Gray_RImage,TH_RImage);

					CBlobLabeling blobs;
					blobs.SetParam(TH_RImage,0.1);
					blobs.DoLabeling(); 

					for( int i=0; i < blobs.m_nBlobs; i++ )
					{	
						 CvPoint pt1 = cvPoint( blobs.m_recBlobs[i].x,blobs.m_recBlobs[i].y );
						 CvPoint pt2 = cvPoint( pt1.x + blobs.m_recBlobs[i].width, pt1.y + blobs.m_recBlobs[i].height );
						 CvPoint ptc = cvPoint( (pt1.x + blobs.m_recBlobs[i].width/2), (pt1.y + blobs.m_recBlobs[i].height/2));
						 // First predict, to update the internal statePre variable

						 Mat prediction_RT = KF_RT.predict();
						 Point predictPt_RT(prediction_RT.at<float>(0),prediction_RT.at<float>(1));
						
						 Mat prediction_RI = KF_RI.predict();
						 Point predictPt_RI(prediction_RI.at<float>(0),prediction_RI.at<float>(1));

						 Mat prediction_RM = KF_RM.predict();
						 Point predictPt_RM(prediction_RM.at<float>(0),prediction_RM.at<float>(1));

						 Mat prediction_LT = KF_LT.predict();
						 Point predictPt_LT(prediction_LT.at<float>(0),prediction_LT.at<float>(1));
						
						 Mat prediction_LI = KF_LI.predict();
						 Point predictPt_LI(prediction_LI.at<float>(0),prediction_LI.at<float>(1));

						 Mat prediction_LM = KF_LM.predict();
						 Point predictPt_LM(prediction_LM.at<float>(0),prediction_LM.at<float>(1));

						 
					     CvPoint pt_00 = cvPoint( blobs.m_recBlobs[0].x,blobs.m_recBlobs[0].y );
						 CvPoint ptc_00 = cvPoint( (pt_00.x + blobs.m_recBlobs[0].width/2), (pt_00.y + blobs.m_recBlobs[0].height/2));

						 CvPoint pt_01 = cvPoint( blobs.m_recBlobs[1].x,blobs.m_recBlobs[1].y );
						 CvPoint ptc_01 = cvPoint( (pt_01.x + blobs.m_recBlobs[1].width/2), (pt_01.y + blobs.m_recBlobs[1].height/2));

						 CvPoint pt_02 = cvPoint( blobs.m_recBlobs[2].x,blobs.m_recBlobs[2].y );
						 CvPoint ptc_02 = cvPoint( (pt_02.x + blobs.m_recBlobs[2].width/2), (pt_02.y + blobs.m_recBlobs[2].height/2));

						 CvPoint pt_03 = cvPoint( blobs.m_recBlobs[3].x,blobs.m_recBlobs[3].y );
						 CvPoint ptc_03 = cvPoint( (pt_03.x + blobs.m_recBlobs[3].width/2), (pt_03.y + blobs.m_recBlobs[3].height/2));

						 CvPoint pt_04 = cvPoint( blobs.m_recBlobs[4].x,blobs.m_recBlobs[4].y );
						 CvPoint ptc_04 = cvPoint( (pt_04.x + blobs.m_recBlobs[4].width/2), (pt_04.y + blobs.m_recBlobs[4].height/2));

						 CvPoint pt_05 = cvPoint( blobs.m_recBlobs[5].x,blobs.m_recBlobs[5].y );
						 CvPoint ptc_05 = cvPoint( (pt_05.x + blobs.m_recBlobs[5].width/2), (pt_05.y + blobs.m_recBlobs[5].height/2));

						 /////////////////////////////
						 CvFont font_Init;
						 cvInitFont( &font_Init, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC, 1.0, 1.0, 0, 1 );

						 cv::Point Point_State;
						 Point_State.x = 15;
						 Point_State.y = 30;
					

						 if(count<=100)
						 {
						 ptc_RThumb = ptc_02;
						 ptc_RIndex = ptc_00;
						 ptc_RMiddle = ptc_01;

						 ptc_LThumb = ptc_05;
						 ptc_LIndex = ptc_03;
						 ptc_LMiddle = ptc_04;

						 cvPutText(image,"Initializing",Point_State,&font_Init,Scalar::all(255));
						 }
						 /////////////////////////////

						 
						 double d01,d02,d03,d04,d05,d06;
						 double d07,d08,d09,d10,d11,d12;
						 double d13,d14,d15,d16,d17,d18;
						 double d19,d20,d21,d22,d23,d24;
						 double d25,d26,d27,d28,d29,d30;
						 double d31,d32,d33,d34,d35,d36;
					
						 if(count>100)
						 {
						 /////R///////
						 d01 = (double)(sqrt((double) (((RT_old.x - ptc_00.x)*(RT_old.x - ptc_00.x)) + ((RT_old.y - ptc_00.y)*(RT_old.y - ptc_00.y)))));
						 d02 = (double)(sqrt((double) (((RT_old.x - ptc_01.x)*(RT_old.x - ptc_01.x)) + ((RT_old.y - ptc_01.y)*(RT_old.y - ptc_01.y)))));
						 d03 = (double)(sqrt((double) (((RT_old.x - ptc_02.x)*(RT_old.x - ptc_02.x)) + ((RT_old.y - ptc_02.y)*(RT_old.y - ptc_02.y)))));
						 d04 = (double)(sqrt((double) (((RT_old.x - ptc_03.x)*(RT_old.x - ptc_03.x)) + ((RT_old.y - ptc_03.y)*(RT_old.y - ptc_03.y)))));
						 d05 = (double)(sqrt((double) (((RT_old.x - ptc_04.x)*(RT_old.x - ptc_04.x)) + ((RT_old.y - ptc_04.y)*(RT_old.y - ptc_04.y)))));
						 d06 = (double)(sqrt((double) (((RT_old.x - ptc_05.x)*(RT_old.x - ptc_05.x)) + ((RT_old.y - ptc_05.y)*(RT_old.y - ptc_05.y)))));

						 d07 = (double)(sqrt((double) (((RI_old.x - ptc_00.x)*(RI_old.x - ptc_00.x)) + ((RI_old.y - ptc_00.y)*(RI_old.y - ptc_00.y)))));
						 d08 = (double)(sqrt((double) (((RI_old.x - ptc_01.x)*(RI_old.x - ptc_01.x)) + ((RI_old.y - ptc_01.y)*(RI_old.y - ptc_01.y)))));
						 d09 = (double)(sqrt((double) (((RI_old.x - ptc_02.x)*(RI_old.x - ptc_02.x)) + ((RI_old.y - ptc_02.y)*(RI_old.y - ptc_02.y)))));
						 d10 = (double)(sqrt((double) (((RI_old.x - ptc_03.x)*(RI_old.x - ptc_03.x)) + ((RI_old.y - ptc_03.y)*(RI_old.y - ptc_03.y)))));
						 d11 = (double)(sqrt((double) (((RI_old.x - ptc_04.x)*(RI_old.x - ptc_04.x)) + ((RI_old.y - ptc_04.y)*(RI_old.y - ptc_04.y)))));
						 d12 = (double)(sqrt((double) (((RI_old.x - ptc_05.x)*(RI_old.x - ptc_05.x)) + ((RI_old.y - ptc_05.y)*(RI_old.y - ptc_05.y)))));

						 d13 = (double)(sqrt((double) (((RM_old.x - ptc_00.x)*(RM_old.x - ptc_00.x)) + ((RM_old.y - ptc_00.y)*(RM_old.y - ptc_00.y)))));
						 d14 = (double)(sqrt((double) (((RM_old.x - ptc_01.x)*(RM_old.x - ptc_01.x)) + ((RM_old.y - ptc_01.y)*(RM_old.y - ptc_01.y)))));
						 d15 = (double)(sqrt((double) (((RM_old.x - ptc_02.x)*(RM_old.x - ptc_02.x)) + ((RM_old.y - ptc_02.y)*(RM_old.y - ptc_02.y)))));
						 d16 = (double)(sqrt((double) (((RM_old.x - ptc_03.x)*(RM_old.x - ptc_03.x)) + ((RM_old.y - ptc_03.y)*(RM_old.y - ptc_03.y)))));
						 d17 = (double)(sqrt((double) (((RM_old.x - ptc_04.x)*(RM_old.x - ptc_04.x)) + ((RM_old.y - ptc_04.y)*(RM_old.y - ptc_04.y)))));
						 d18 = (double)(sqrt((double) (((RM_old.x - ptc_05.x)*(RM_old.x - ptc_05.x)) + ((RM_old.y - ptc_05.y)*(RM_old.y - ptc_05.y)))));

						 //////////L////////
						 d19 = (double)(sqrt((double) (((LT_old.x - ptc_00.x)*(LT_old.x - ptc_00.x)) + ((LT_old.y - ptc_00.y)*(LT_old.y - ptc_00.y)))));
						 d20 = (double)(sqrt((double) (((LT_old.x - ptc_01.x)*(LT_old.x - ptc_01.x)) + ((LT_old.y - ptc_01.y)*(LT_old.y - ptc_01.y)))));
						 d21 = (double)(sqrt((double) (((LT_old.x - ptc_02.x)*(LT_old.x - ptc_02.x)) + ((LT_old.y - ptc_02.y)*(LT_old.y - ptc_02.y)))));
						 d22 = (double)(sqrt((double) (((LT_old.x - ptc_03.x)*(LT_old.x - ptc_03.x)) + ((LT_old.y - ptc_03.y)*(LT_old.y - ptc_03.y)))));
						 d23 = (double)(sqrt((double) (((LT_old.x - ptc_04.x)*(LT_old.x - ptc_04.x)) + ((LT_old.y - ptc_04.y)*(LT_old.y - ptc_04.y)))));
						 d24 = (double)(sqrt((double) (((LT_old.x - ptc_05.x)*(LT_old.x - ptc_05.x)) + ((LT_old.y - ptc_05.y)*(LT_old.y - ptc_05.y)))));

						 d25 = (double)(sqrt((double) (((LI_old.x - ptc_00.x)*(LI_old.x - ptc_00.x)) + ((LI_old.y - ptc_00.y)*(LI_old.y - ptc_00.y)))));
						 d26 = (double)(sqrt((double) (((LI_old.x - ptc_01.x)*(LI_old.x - ptc_01.x)) + ((LI_old.y - ptc_01.y)*(LI_old.y - ptc_01.y)))));
						 d27 = (double)(sqrt((double) (((LI_old.x - ptc_02.x)*(LI_old.x - ptc_02.x)) + ((LI_old.y - ptc_02.y)*(LI_old.y - ptc_02.y)))));
						 d28 = (double)(sqrt((double) (((LI_old.x - ptc_03.x)*(LI_old.x - ptc_03.x)) + ((LI_old.y - ptc_03.y)*(LI_old.y - ptc_03.y)))));
						 d29 = (double)(sqrt((double) (((LI_old.x - ptc_04.x)*(LI_old.x - ptc_04.x)) + ((LI_old.y - ptc_04.y)*(LI_old.y - ptc_04.y)))));
						 d30 = (double)(sqrt((double) (((LI_old.x - ptc_05.x)*(LI_old.x - ptc_05.x)) + ((LI_old.y - ptc_05.y)*(LI_old.y - ptc_05.y)))));

						 d31 = (double)(sqrt((double) (((LM_old.x - ptc_00.x)*(LM_old.x - ptc_00.x)) + ((LM_old.y - ptc_00.y)*(LM_old.y - ptc_00.y)))));
						 d32 = (double)(sqrt((double) (((LM_old.x - ptc_01.x)*(LM_old.x - ptc_01.x)) + ((LM_old.y - ptc_01.y)*(LM_old.y - ptc_01.y)))));
						 d33 = (double)(sqrt((double) (((LM_old.x - ptc_02.x)*(LM_old.x - ptc_02.x)) + ((LM_old.y - ptc_02.y)*(LM_old.y - ptc_02.y)))));
						 d34 = (double)(sqrt((double) (((LM_old.x - ptc_03.x)*(LM_old.x - ptc_03.x)) + ((LM_old.y - ptc_03.y)*(LM_old.y - ptc_03.y)))));
						 d35 = (double)(sqrt((double) (((LM_old.x - ptc_04.x)*(LM_old.x - ptc_04.x)) + ((LM_old.y - ptc_04.y)*(LM_old.y - ptc_04.y)))));
						 d36 = (double)(sqrt((double) (((LM_old.x - ptc_05.x)*(LM_old.x - ptc_05.x)) + ((LM_old.y - ptc_05.y)*(LM_old.y - ptc_05.y)))));

						 

						///// Distance Compare
						 /////////////////R/////////////////
						double distance_RT[6] = { d01 , d02 , d03, d04 , d05 , d06 };
						int m_RT, n_RT;
						double temp_RT;

							for(m_RT = 0; m_RT < 6; m_RT++)
							{
								for(n_RT = 0; n_RT < (6 - m_RT); n_RT++)
								{
									if(distance_RT[n_RT] < distance_RT[n_RT + 1])
									{
										temp_RT = distance_RT[n_RT];
										distance_RT[n_RT] = distance_RT[n_RT + 1];
										distance_RT[n_RT + 1] = temp_RT;
									}
								}
							}
							////

						double distance_RI[6] = { d07 , d08 , d09 , d10 , d11 ,d12 };
						int m_RI, n_RI;
						double temp_RI;

							for(m_RI = 0; m_RI < 6; m_RI++)
							{
								for(n_RI = 0; n_RI < (6 - m_RI); n_RI++)
								{
									if(distance_RI[n_RI] < distance_RI[n_RI + 1])
									{
										temp_RI = distance_RI[n_RI];
										distance_RI[n_RI] = distance_RI[n_RI + 1];
										distance_RI[n_RI + 1] = temp_RI;
									}
								}
							}

							////

						double distance_RM[6] = { d13 , d14 , d15 , d16 , d17 , d18 };
						int m_RM, n_RM;
						double temp_RM;

							for(m_RM = 0; m_RM < 6; m_RM++)
							{
								for(n_RM = 0; n_RM < (6 - m_RM); n_RM++)
								{
									if(distance_RM[n_RM] < distance_RM[n_RM + 1])
									{
										temp_RM = distance_RM[n_RM];
										distance_RM[n_RM] = distance_RM[n_RM + 1];
										distance_RM[n_RM + 1] = temp_RM;
									}
								}
							}


							////////////////L///////////////
						double distance_LT[6] = { d19 , d20 , d21, d22 , d23 , d24 };
						int m_LT, n_LT;
						double temp_LT;

							for(m_LT = 0; m_LT < 6; m_LT++)
							{
								for(n_LT = 0; n_LT < (6 - m_LT); n_LT++)
								{
									if(distance_LT[n_LT] < distance_LT[n_LT + 1])
									{
										temp_LT = distance_LT[n_LT];
										distance_LT[n_LT] = distance_LT[n_LT + 1];
										distance_LT[n_LT + 1] = temp_LT;
									}
								}
							}
							////

						double distance_LI[6] = { d25 , d26 , d27 , d28 , d29 ,d30 };
						int m_LI, n_LI;
						double temp_LI;

							for(m_LI = 0; m_LI < 6; m_LI++)
							{
								for(n_LI = 0; n_LI < (6 - m_LI); n_LI++)
								{
									if(distance_LI[n_LI] < distance_LI[n_LI + 1])
									{
										temp_LI = distance_LI[n_LI];
										distance_LI[n_LI] = distance_LI[n_LI + 1];
										distance_LI[n_LI + 1] = temp_LI;
									}
								}
							}

							////

						double distance_LM[6] = { d31 , d32 , d33 , d34 , d35 , d36 };
						int m_LM, n_LM;
						double temp_LM;

							for(m_LM = 0; m_LM < 6; m_LM++)
							{
								for(n_LM = 0; n_LM < (6 - m_LM); n_LM++)
								{
									if(distance_LM[n_LM] < distance_LM[n_LM + 1])
									{
										temp_LM = distance_LM[n_LM];
										distance_LM[n_LM] = distance_LM[n_LM + 1];
										distance_LM[n_LM + 1] = temp_LM;
									}
								}
							}

							///////////////////////////////////////////////////////////////////////////
							/////R//////////
						    if(distance_RT[5] == d01) ptc_RThumb = ptc_00;
							else if(distance_RT[5] == d02) ptc_RThumb = ptc_01;
							else if(distance_RT[5] == d03) ptc_RThumb = ptc_02;
							else if(distance_RT[5] == d04) ptc_RThumb = ptc_03;
							else if(distance_RT[5] == d05) ptc_RThumb = ptc_04;
							else ptc_RThumb = ptc_05;

							if(distance_RI[5] == d07) ptc_RIndex = ptc_00;
							else if(distance_RI[5] == d08) ptc_RIndex = ptc_01;
							else if(distance_RI[5] == d09) ptc_RIndex = ptc_02;
							else if(distance_RI[5] == d10) ptc_RIndex = ptc_03;
							else if(distance_RI[5] == d11) ptc_RIndex = ptc_04;
							else ptc_RIndex = ptc_05;

							if(distance_RM[5] == d13) ptc_RMiddle = ptc_00;
							else if(distance_RM[5] == d14) ptc_RMiddle = ptc_01;
							else if(distance_RM[5] == d15) ptc_RMiddle = ptc_02;
							else if(distance_RM[5] == d16) ptc_RMiddle = ptc_03;
							else if(distance_RM[5] == d17) ptc_RMiddle = ptc_04;
							else ptc_RMiddle = ptc_05;
							
							///L/////////////
							if(distance_LT[5] == d19) ptc_LThumb = ptc_00;
							else if(distance_LT[5] == d20) ptc_LThumb = ptc_01;
							else if(distance_LT[5] == d21) ptc_LThumb = ptc_02;
							else if(distance_LT[5] == d22) ptc_LThumb = ptc_03;
							else if(distance_LT[5] == d23) ptc_LThumb = ptc_04;
							else ptc_LThumb = ptc_05;

							if(distance_LI[5] == d25) ptc_LIndex = ptc_00;
							else if(distance_LI[5] == d26) ptc_LIndex = ptc_01;
							else if(distance_LI[5] == d27) ptc_LIndex = ptc_02;
							else if(distance_LI[5] == d28) ptc_LIndex = ptc_03;
							else if(distance_LI[5] == d29) ptc_LIndex = ptc_04;
							else ptc_LIndex = ptc_05;

							if(distance_LM[5] == d31) ptc_LMiddle = ptc_00;
							else if(distance_LM[5] == d32) ptc_LMiddle = ptc_01;
							else if(distance_LM[5] == d33) ptc_LMiddle = ptc_02;
							else if(distance_LM[5] == d34) ptc_LMiddle = ptc_03;
							else if(distance_LM[5] == d35) ptc_LMiddle = ptc_04;
							else ptc_LMiddle = ptc_05;

						 }

						 double l_RTI, l_RIM, l_RMT;
					     double l_LTI, l_LIM, l_LMT;

						 l_RTI = (double)(sqrt((double) (((ptc_RThumb.x - ptc_RIndex.x)*(ptc_RThumb.x - ptc_RIndex.x)) + ((ptc_RThumb.y - ptc_RIndex.y)*(ptc_RThumb.y - ptc_RIndex.y)))));
						 l_RIM = (double)(sqrt((double) (((ptc_RIndex.x - ptc_RMiddle.x)*(ptc_RIndex.x - ptc_RMiddle.x)) + ((ptc_RIndex.y - ptc_RMiddle.y)*(ptc_RIndex.y - ptc_RMiddle.y)))));
						 l_RMT = (double)(sqrt((double) (((ptc_RMiddle.x - ptc_RThumb.x)*(ptc_RMiddle.x - ptc_RThumb.x)) + ((ptc_RMiddle.y - ptc_RThumb.y)*(ptc_RMiddle.y - ptc_RThumb.y)))));

						 l_LTI = (double)(sqrt((double) (((ptc_LThumb.x - ptc_LIndex.x)*(ptc_LThumb.x - ptc_LIndex.x)) + ((ptc_LThumb.y - ptc_LIndex.y)*(ptc_LThumb.y - ptc_LIndex.y)))));
						 l_LIM = (double)(sqrt((double) (((ptc_LIndex.x - ptc_LMiddle.x)*(ptc_LIndex.x - ptc_LMiddle.x)) + ((ptc_LIndex.y - ptc_LMiddle.y)*(ptc_LIndex.y - ptc_LMiddle.y)))));
						 l_LMT = (double)(sqrt((double) (((ptc_LMiddle.x - ptc_LThumb.x)*(ptc_LMiddle.x - ptc_LThumb.x)) + ((ptc_LMiddle.y - ptc_LThumb.y)*(ptc_LMiddle.y - ptc_LThumb.y)))));

						 int line_r[3] = { l_RTI , l_RIM , l_RMT};
						 int m_r, n_r;
						 int temp_r;

							for(m_r = 0; m_r < 3; m_r++)
							{
								for(n_r = 0; n_r < (3 - m_r); n_r++)
								{
									if(line_r[n_r] < line_r[n_r + 1])
									{
										temp_r = line_r[n_r];
										line_r[n_r] = line_r[n_r + 1];
										line_r[n_r + 1] = temp_r;
									}
								}
							}


						 int line_l[3] = { l_LTI , l_LIM , l_LMT};
						 int m_l, n_l;
						 int temp_l;

							for(m_l = 0; m_l < 3; m_l++)
							{
								for(n_l = 0; n_l < (3 - m_l); n_l++)
								{
									if(line_l[n_l] < line_l[n_l + 1])
									{
										temp_l = line_l[n_l];
										line_l[n_l] = line_l[n_l + 1];
										line_l[n_l + 1] = temp_l;
									}
								}
							}


						 cv::Point ptc_RC;
						 ptc_RC.x = (ptc_RThumb.x + ptc_RIndex.x + ptc_RMiddle.x)/3;
						 ptc_RC.y = (ptc_RThumb.y + ptc_RIndex.y + ptc_RMiddle.y)/3;

						 cv::Point ptc_LC;
						 ptc_LC.x = (ptc_LThumb.x + ptc_LIndex.x + ptc_LMiddle.x)/3;
						 ptc_LC.y = (ptc_LThumb.y + ptc_LIndex.y + ptc_LMiddle.y)/3;

						 int r_radius_out;
						 r_radius_out = (line_r[0]);
						 cvCircle( image, ptc_RC, r_radius_out, Scalar(0,0,150), 1, 8, 0 );

						 int l_radius_out;
						 l_radius_out = (line_l[0]);
						 cvCircle( image, ptc_LC, l_radius_out, Scalar(150,0,0), 1, 8, 0 );


						 measurement_RT(0) = ptc_RThumb.x;
						 measurement_RT(1) = ptc_RThumb.y;
  
						 measurement_RI(0) = ptc_RIndex.x;
						 measurement_RI(1) = ptc_RIndex.y;

						 measurement_RM(0) = ptc_RMiddle.x;
						 measurement_RM(1) = ptc_RMiddle.y;
						 //////////////
						 measurement_LT(0) = ptc_LThumb.x;
						 measurement_LT(1) = ptc_LThumb.y;
  
						 measurement_LI(0) = ptc_LIndex.x;
						 measurement_LI(1) = ptc_LIndex.y;

						 measurement_LM(0) = ptc_LMiddle.x;
						 measurement_LM(1) = ptc_LMiddle.y;



						 // The update phase 
						 Mat estimated_RT = KF_RT.correct(measurement_RT);
						 Mat estimated_RI = KF_RI.correct(measurement_RI);
						 Mat estimated_RM = KF_RM.correct(measurement_RM);

						 Mat estimated_LT = KF_LT.correct(measurement_LT);
						 Mat estimated_LI = KF_LI.correct(measurement_LI);
						 Mat estimated_LM = KF_LM.correct(measurement_LM);
 
						 Point statePt_RT(estimated_RT.at<float>(0),estimated_RT.at<float>(1));
						 Point measPt_RT(measurement_RT(0),measurement_RT(1));

						 Point statePt_RI(estimated_RI.at<float>(0),estimated_RI.at<float>(1));
						 Point measPt_RI(measurement_RI(0),measurement_RI(1));

						 Point statePt_RM(estimated_RM.at<float>(0),estimated_RM.at<float>(1));
						 Point measPt_RM(measurement_RM(0),measurement_RM(1));
						 //////
						 Point statePt_LT(estimated_LT.at<float>(0),estimated_LT.at<float>(1));
						 Point measPt_LT(measurement_LT(0),measurement_LT(1));

						 Point statePt_LI(estimated_LI.at<float>(0),estimated_LI.at<float>(1));
						 Point measPt_LI(measurement_LI(0),measurement_LI(1));

						 Point statePt_LM(estimated_LM.at<float>(0),estimated_LM.at<float>(1));
						 Point measPt_LM(measurement_LM(0),measurement_LM(1));


						 LT_old = ptc_LThumb;
						 LI_old = ptc_LIndex;
						 LM_old = ptc_LMiddle;

						 RT_old = ptc_RThumb;
						 RI_old = ptc_RIndex;
						 RM_old = ptc_RMiddle;

						 double cam_scale_x = 3.0;
						 double cam_scale_y = 3.0;
						 SetCursorPos(ptc_RC.x*cam_scale_x, ptc_RC.y*cam_scale_y);

						int radius_c = 1.0;
						cvCircle( image, ptc_RC, radius_c, Scalar::all(255), 3, 8, 0 ); // R
						cvCircle( image, ptc_LC, radius_c, Scalar::all(255), 3, 8, 0 ); // L

						cv::Point state_RC;
						state_RC.x = (statePt_RT.x + statePt_RI.x + statePt_RM.x)/3;
						state_RC.y = (statePt_RT.y + statePt_RI.y + statePt_RM.y)/3;

						cv::Point state_LC;
						state_LC.x = (statePt_LT.x + statePt_LI.x + statePt_LM.x)/3;
						state_LC.y = (statePt_LT.y + statePt_LI.y + statePt_LM.y)/3;
						
						CvFont font_func;
						cvInitFont( &font_func, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC, 1.0, 1.0, 0, 1 );

						//if ((ptc_LThumb.x<=ptc_LC.x-20)&&(l_radius_out<100))
						//{
						//	//mouse_event(MOUSEEVENTF_WHEEL, 0, 0, SCROLLDOWN, 0);
						//	//cvPutText(image,"(Scroll Down)",Point_State,&font_Init,Scalar::all(255));
						//}
						//if ((ptc_LThumb.x<=ptc_LC.x-20)&&(l_radius_out>150))
						//{
						//	//mouse_event(MOUSEEVENTF_WHEEL, 0, 0, SCROLLUP, 0);
						//	//cvPutText(image,"(Scroll Up)",Point_State,&font_Init,Scalar::all(255));
						//}
						//if((ptc_LThumb.x>ptc_LC.x-10)&&(l_LTI<=40)) 
						//{
						//	//mouse_event(MOUSEEVENTF_LEFTDOWN, 100, 100, 0, 0); 
						//	//cvPutText(image,"(Left Click)",Point_State,&font_Init,Scalar::all(255));
						//}
						//if((ptc_LThumb.x>ptc_LC.x-10)&&(l_LTI>100))
						//{
						//	//mouse_event(MOUSEEVENTF_LEFTUP, 100, 100, 0, 0); 
						//}
						//if((ptc_LThumb.x>ptc_LC.x-10)&&(l_LMT<=30))
						//{
						//	//mouse_event(MOUSEEVENTF_RIGHTUP, 100, 100, 0, 0); 
						//	//cvPutText(image,"(Right Click)",Point_State,&font_Init,Scalar::all(255));
						//}

						////////////////////Save Data

						// Draw
						cvDrawCircle(image,statePt_RT,5,Scalar(0,0,255),1,8,0);
						cvDrawCircle(image,measPt_RT,5,Scalar(255,255,255),1,8,0);
						cvDrawCircle(image,statePt_RI,5,Scalar(0,255,0),1,8,0);
						cvDrawCircle(image,measPt_RI,5,Scalar(255,255,255),1,8,0);
						cvDrawCircle(image,statePt_RM,5,Scalar(255,0,0),1,8,0);
						cvDrawCircle(image,measPt_RM,5,Scalar(255,255,255),1,8,0);
						/////////////////

						cvDrawCircle(image,statePt_LT,5,Scalar(0,0,255),1,8,0);
						cvDrawCircle(image,measPt_LT,5,Scalar(255,255,255),1,8,0);
						cvDrawCircle(image,statePt_LI,5,Scalar(0,255,0),1,8,0);
						cvDrawCircle(image,measPt_LI,5,Scalar(255,255,255),1,8,0);
						cvDrawCircle(image,statePt_LM,5,Scalar(255,0,0),1,8,0);
						cvDrawCircle(image,measPt_LM,5,Scalar(255,255,255),1,8,0);


						////Line
						cvLine(image,measPt_RT,measPt_RI,Scalar::all(255),1,8,0);
						cvLine(image,measPt_RI,measPt_RM,Scalar::all(255),1,8,0);
						cvLine(image,measPt_RM,measPt_RT,Scalar::all(255),1,8,0);

						cvLine(image,measPt_LT,measPt_LI,Scalar::all(255),1,8,0);
						cvLine(image,measPt_LI,measPt_LM,Scalar::all(255),1,8,0);
						cvLine(image,measPt_LM,measPt_LT,Scalar::all(255),1,8,0);

						cvLine(image,statePt_RT,statePt_RI,Scalar(0,0,255),1,8,0);
						cvLine(image,statePt_RI,statePt_RM,Scalar(0,0,255),1,8,0);
						cvLine(image,statePt_RM,statePt_RT,Scalar(0,0,255),1,8,0);

						cvLine(image,statePt_LT,statePt_LI,Scalar(255,0,0),1,8,0);
						cvLine(image,statePt_LI,statePt_LM,Scalar(255,0,0),1,8,0);
						cvLine(image,statePt_LM,statePt_LT,Scalar(255,0,0),1,8,0);
						
						/// Text
						CvFont font;
						cvInitFont( &font, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC, 0.5, 0.5, 0, 1 );

						cvPutText(image,"   R_Thumb",statePt_RT,&font,Scalar(0,0,255));
						cvPutText(image,"   R_Index",statePt_RI,&font,Scalar(0,255,0));
						cvPutText(image,"   R_Middle",statePt_RM,&font,Scalar(255,0,0));

						cvPutText(image,"   L_Thumb",statePt_LT,&font,Scalar(0,0,255));
						cvPutText(image,"   L_Index",statePt_LI,&font,Scalar(0,255,0));
						cvPutText(image,"   L_Middle",statePt_LM,&font,Scalar(255,0,0));
						cv::Mat Image = image;

						cvNamedWindow("Point Recognition",1); 
						cvMoveWindow("Point Recognition", 0, 0);
						//cvResizeWindow("Point Recognition", 960, 540);
						char savefile[200];
						cvShowImage("Point Recognition", image);
						sprintf(savefile, "image%d.jpg", count++);
						imwrite(savefile, Image); 


						count+=1;
						if( cvWaitKey(10) >= 0 )
						break;
	
						}
					}
				}
			}
		}
	}
	

	cvDestroyWindow("Point Recognition");
	cvReleaseCapture( &capture );
    return 0;
}
