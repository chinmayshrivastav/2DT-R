    #include <ros/ros.h>
    #include <image_transport/image_transport.h>
    #include <cv_bridge/cv_bridge.h>
    #include <sensor_msgs/image_encodings.h>
    #include <opencv2/imgproc/imgproc.hpp>
    #include <opencv2/highgui/highgui.hpp>
    
    static const std::string OPENCV_WINDOW = "Image window";
    
   class RectDetector
   {
     ros::NodeHandle nh_;
     image_transport::ImageTransport it_;
     image_transport::Subscriber image_sub_;
     image_transport::Publisher image_pub_;
   
   public:
     RectDetector()
       : it_(nh_)
     {
       // Subscribe to input video feed and publish output video feed
       image_sub_ = it_.subscribe("/image_raw", 1,
         &RectDetector::imageCb, this);
       image_pub_ = it_.advertise("/rect_detector/output_video", 1);
  
       cv::namedWindow(OPENCV_WINDOW);
     }
   
     ~RectDetector()
     {
       cv::destroyWindow(OPENCV_WINDOW);
     }
   
     void imageCb(const sensor_msgs::ImageConstPtr& msg)
     {
       cv_bridge::CvImagePtr cv_ptr;
       try
       {
         cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
       }
       catch (cv_bridge::Exception& e)
       {
         ROS_ERROR("cv_bridge exception: %s", e.what());
         return;
       }

       //Image processing
       //For Calculating Computation Time saving TickFrequency before starting image processing t0 and then noting TickFreq after processing t1
        int64 t0 = cv::getTickCount();
     	
        Mat pyr, timg, gray0(image.size(), CV_8U), gray;
        vector<vector<Point> > squares;
        pyrDown(cv_ptr->image, pyr, Size(cv_ptr->image.cols/2, cv_ptr->image.rows/2));
        pyrUp(pyr, timg, cv_ptr->image.size());       
        medianBlur(cv_ptr->image, timg, 9)
        vector<vector<Point> > contours;

        for( int c = 0; c < 3; c++ )
           {
             int ch[] = {c, 0};
             mixChannels(&timg, 1, &gray0, 1, ch, 1);

        // try several threshold levels
        for( int l = 0; l < N; l++ )
        {
            
            if( l == 0 )
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                Canny(gray0, gray, 5, thresh, 5);
                // dilate canny output to remove potential
                // holes between edge segments
                dilate(gray, gray, Mat(), Point(-1,-1));
            }
            else
            {
                gray = gray0 >= (l+1)*255/N;
            }

            // find contours and store them all as a list
            findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
            vector<Point> approx;

            // test each contour
            for( size_t i = 0; i < contours.size(); i++ )
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);
                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area as it can be positive or negative
                if( approx.size() == 4 &&
                    fabs(contourArea(Mat(approx))) > 1000 &&
                    isContourConvex(Mat(approx)) )
                {
                    double maxCosine = 0;

                    for( int j = 2; j < 5; j++ )
                    {
                        // find the maximum cosine of the angle between joint edges
                        double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if( maxCosine < 0.3 )
                        squares.push_back(approx);
                     
                }
            }
        }
    }
}
        int64 t1 = cv::getTickCount();
        double secs = (t1 - t0)/cv::getTickFrequency();
        cout<<secs<<endl;
       
       //Draw boxes on images

       for( size_t i = 0; i < squares.size(); i++ )
        {
        const Point* p = &squares[i][0];

        int n = (int)squares[i].size();
        //dont detect the border
        if (p-> x > 3 && p->y > 3)
          polylines(cv_ptr->image, &p, &n, 1, true, Scalar(0,255,0), 3, LINE_AA);
        }




       // Update GUI Window
       cv::imshow(OPENCV_WINDOW, cv_ptr->image);
       cv::waitKey(3);
   
       // Output modified video stream
       image_pub_.publish(cv_ptr->toImageMsg());
     }
   };
   
   int main(int argc, char** argv)
   {
     ros::init(argc, argv, "rect_detector");
     RectDetector ic;
     ros::spin();
     return 0;
   }
