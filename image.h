#ifndef IMAGE_FILTERING
#define IMAGE_FILTERING
#include <opencv2/opencv.hpp>
#include <gtk/gtk.h>
#include <iostream>
#include <vector>
#include <sys/stat.h> 
#include <cstring> 
#include "utils.h"

#define Median_Window_Size_Image 3

using namespace cv;
using namespace std;

void save_filtered_image(const Mat& filtered_image, const string& original_filename);
int get_median(vector<int> &values);
void apply_median_filter(Mat &image);
void load_image(const string& filename);
void on_image_input_clicked(GtkWidget *widget, gpointer data);


#endif
