#ifndef VISION_H
#define VISION_H

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
//#include <json/json.h>


class Vision {
 public:
  Vision(void);
  ~Vision(void);
  int loop(cv::Mat src);
 private:
  static const int kNumColors = 2;
  static const int kRobotRectX = 80;
  static const int kRobotRectY = 180;
  static const int kRobotRectWidth = 180;
  static const int kRobotRectHeight = 60;
  cv::Vec3b _getPixel(const cv::Mat img, const int x, const int y);
  void _thresholdImage(const cv::Mat img, const cv::Vec3b pixel, cv::Mat &output);
  int _sockfd;
  cv::Vec3b _color[kNumColors];
  cv::Vec3b _true_color[kNumColors];
};
#endif // VISION_H
