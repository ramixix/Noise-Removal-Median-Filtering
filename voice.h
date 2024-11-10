#ifndef VOICE_FILTERING
#define VOICE_FILTERING
#include <opencv2/opencv.hpp>
#include <gtk/gtk.h>
#include <vector>
#include <sys/stat.h> 
#include <sndfile.h>
#include "utils.h"

#define Median_Window_Size_Voice 3

using namespace cv;
using namespace std;


void apply_median_filter(vector<float> &samples, int filter_size);
void apply_median_filter_to_audio(const char *filepath);
void on_audio_input_clicked(GtkWidget *widget, gpointer data);

#endif
