    #include <ros/ros.h>
    #include <image_transport/image_transport.h>
    #include <opencv2/highgui/highgui.hpp>
    #include <cv_bridge/cv_bridge.h>
    #include <sstream>
//  #include<"tag_detector/set_blur_window_size.h">

/*    bool setwindow(tag_detector::set_blur_window_size::Request  &req,
             tag_detector::set_blur_window_size::Response &res)
    {
     res.Size[a,b] = req.Size[x,y];
     ROS_INFO("request: x=%ld, y=%ld", (long int)req.a, (long int)req.b);
     ROS_INFO("sending back response: a=%ld, b=%ld", (long int)res.Size[0],(long int)res.Size[1];
     return true;
    }    
    
*/
    int main(int argc, char** argv)
    {
       ros::init(argc, argv, "publisher");
//     ros::init(argc, argv, "service_server");
       ros::NodeHandle nh;
//     ros::ServiceServer service_server = nh.advertiseService("tag_detector",setwindow);
       image_transport::ImageTransport it(nh);
       image_transport::Publisher pub = it.advertise("/image_raw", 1);
     
       //Loading video file
       VideoCapture cap(const String& video.avi); // video file
       if(!cap.isOpened())  // check if we succeeded
        return -1;
       sensor_msgs::ImagePtr msg;
       ros::Rate loop_rate(5);
       Mat frame;
       Mat edges;
       while(nk.ok()){
        cap >> frame;
        cvtColor(frame, edges, CV_BGR2GRAY);
        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
       // Check if grabbed frame is actually full with some content
       if(!edges.empty()) {
         msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", edges).toImageMsg();
         pub.publish(msg);
         cv::waitKey(1);
       }

       ros::spinOnce();
       loop_rate.sleep();
     
   }	
