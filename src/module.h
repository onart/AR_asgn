#ifndef __MODULE_H__
#define __MODULE_H__

#ifndef _DEBUG
#pragma comment(lib,"opencv2/lib/opencv_imgcodecs455.lib")
#pragma comment(lib,"opencv2/lib/opencv_highgui455.lib")
#pragma comment(lib, "opencv2/lib/opencv_core455.lib")
#pragma comment(lib, "opencv2/lib/opencv_videoio455.lib")
#pragma comment(lib, "opencv2/lib/opencv_aruco455.lib")
#pragma comment(lib, "opencv2/lib/opencv_calib3d455.lib")
#else
#pragma comment(lib,"opencv2/lib/opencv_imgcodecs455d.lib")
#pragma comment(lib,"opencv2/lib/opencv_highgui455d.lib")
#pragma comment(lib, "opencv2/lib/opencv_core455d.lib")
#pragma comment(lib, "opencv2/lib/opencv_videoio455d.lib")
#pragma comment(lib, "opencv2/lib/opencv_aruco455d.lib")
#pragma comment(lib, "opencv2/lib/opencv_calib3d455d.lib")
#endif

#ifdef _WIN32
#ifdef _WIN64
#include "externals/gl/glfw/win64/glfw3.h"
#pragma comment(lib, "externals/gl/glfw/win64/glfw3_mt.lib")
#else
#include "externals/gl/glfw/win32/glfw3.h"
#pragma comment(lib, "externals/gl/glfw/win32/glfw3_mt.lib")
#endif // _WIN64
#endif // _WIN32

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/aruco.hpp"
#include "opencv2/aruco/charuco.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/calib3d.hpp"
#include <cstdio>
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>
#include <filesystem>

#endif