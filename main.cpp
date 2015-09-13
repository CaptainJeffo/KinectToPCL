#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <pcl/io/pcd_io.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <memory>
#include "IPointCloudDivider.h"
#include "ProjectionDivider.h"
#include "PointCloudBuilder.h"

using namespace std;

cv::Mat reload_32f_image(string filename);
void print_depth(cv::Mat depthMat);
boost::shared_ptr<pcl::visualization::PCLVisualizer> rgbVis (pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr cloud);
PointCloudPtr getSubXCloud(PointCloudPtr cloud, double fromX, double toX);

int main()
{
    cv::Mat depthMat = reload_32f_image("/Users/gjc13/KinectData/depth.bin");
    cv::Mat imageMat = cv::imread("/Users/gjc13/KinectData/registered.png");
    cout << depthMat.at<float>(423, 0) << endl;
    PointCloudBuilder builder(depthMat, imageMat);
    PointCloudPtr pointCloud = builder.getPointCloud();
    cout << pointCloud->points.size() << endl;
//    PointCloudPtr subCloud = getSubXCloud(pointCloud, 4, 20);
//    ProjectionDivider divider(pointCloud);
//    divider.saveDensity();
    auto viewer = rgbVis(pointCloud);
    while (!viewer->wasStopped ())
    {
        viewer->spinOnce (100);
        boost::this_thread::sleep (boost::posix_time::microseconds (100000));
    }
    return 0;
}

//恢复CV::32fc1的深度图
cv::Mat reload_32f_image(string filename)
{
    ifstream fin(filename, ifstream::binary);
    int num_rows, num_cols;
    fin.read((char *) &num_rows, sizeof(int));
    fin.read((char *) &num_cols, sizeof(int));
    cv::Mat mat = cv::Mat::zeros(num_rows, num_cols, CV_32FC1);
    fin.read((char *) mat.data, num_cols * num_rows * 4);
    return mat;
}

void print_depth(cv::Mat depthMat)
{
    int x1 = 205, x2 = 377, y1 = 125, y2 = 229;
    cv::Mat mean;
    cv::Mat stddev;
    cv::Mat boardDepth(depthMat, cv::Range(y1, y2), cv::Range(x1, x2));
    cv::meanStdDev(boardDepth, mean, stddev);
    cout << mean.at<double>(0) << endl;
    cout << stddev.at<double>(0) << endl;
    cv::imshow("depth", boardDepth);
}

boost::shared_ptr<pcl::visualization::PCLVisualizer> rgbVis (pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr cloud)
{
    // --------------------------------------------
    // -----Open 3D viewer and add point cloud-----
    // --------------------------------------------
    boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer (new pcl::visualization::PCLVisualizer ("3D Viewer"));
    viewer->setBackgroundColor (0, 0, 0);
    pcl::visualization::PointCloudColorHandlerRGBField<pcl::PointXYZRGB> rgb(cloud);
    //viewer->addPointCloud<pcl::PointXYZRGB> (cloud, rgb, "sample cloud");
    //viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "sample cloud");
    viewer->addCoordinateSystem (1.0);
    viewer->initCameraParameters ();
    return (viewer);
}

PointCloudPtr getSubXCloud(PointCloudPtr cloud, double fromX, double toX)
{
    PointCloudPtr newCloud(new pcl::PointCloud<pcl::PointXYZRGB>());
    for(pcl::PointXYZRGB point: cloud->points)
    {
        if(point.x < toX & point.x > fromX)
        {
            newCloud->points.push_back(point);
        }
    }
    newCloud->width = (int)newCloud->points.size();
    newCloud->height = 1;
    return newCloud;
}
