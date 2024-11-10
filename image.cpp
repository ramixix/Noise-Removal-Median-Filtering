#include "image.h"

void save_filtered_image(const Mat& filtered_image, const string& original_filename) {
    const char* dir_name = "median_filtered_images";
    
    // Create directory if it doesn't exist
    struct stat st;
    if (stat(dir_name, &st) != 0) {
        mkdir(dir_name, 0777);
    }

    // Build the output file path
    string output_file = string(dir_name) + "/" + original_filename;
    
    // Save the filtered image
    imwrite(output_file, filtered_image);
}


// Helper function to get the median of a vector
int get_median(vector<int> &values) {
    sort(values.begin(), values.end());
    return values[values.size() / 2];
}

// Median filter function
void apply_median_filter(Mat &image) {
    Mat result = image.clone();
    int window_size = Median_Window_Size_Image;
    int border = window_size / 2;

    for (int y = border; y < image.rows - border; y++) {
        for (int x = border; x < image.cols - border; x++) {
            vector<int> window;
            for (int dy = -border; dy <= border; dy++) {
                for (int dx = -border; dx <= border; dx++) {
                    int pixel_value = image.at<uchar>(y + dy, x + dx);
                    window.push_back(pixel_value);
                }
            }
            int median_value = get_median(window);
            result.at<uchar>(y, x) = median_value;
        }
    }

    image = result; // Update the original image with the filtered result
}

// Load the image using OpenCV
void load_image(const string& filename) {
    Mat image = imread(filename, IMREAD_GRAYSCALE);
    if (image.empty()) {
        cout << "Error loading image: " << filename << endl;
        return;
    }

    cout << "Image loaded: " << filename << endl;

    apply_median_filter(image);

    string original_filename = filename.substr(filename.find_last_of("/\\") + 1);
    save_filtered_image(image, original_filename);

    // Save the processed image
    string message = "Image uploaded and filtered. Result saved in 'median_filtered_images'.";
    show_popup(message.c_str());
    cout << "Filtered image saved in 'median_filtered_images'." << endl;
}



// Callback for Image Input Button
void on_image_input_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    const char *valid_extensions[] = {"jpg", "jpeg", "png", "bmp"};

    dialog = gtk_file_chooser_dialog_new("Select an Image",
                                         GTK_WINDOW(data),
                                         action,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_ACCEPT,
                                         NULL);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_mime_type(filter, "image/jpeg");
    gtk_file_filter_add_mime_type(filter, "image/png");
    gtk_file_filter_add_mime_type(filter, "image/bmp");
    gtk_file_filter_set_name(filter, "Image Files");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);

        // Validate the file extension
        if (check_file_extension(filename, valid_extensions, 4)) {
		printf("Selected Image: %s\n", filename);
		load_image(filename);
		printf("Median Filtering Is Done Your Image Is Updated");
        } else {
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                            GTK_DIALOG_DESTROY_WITH_PARENT,
                                                            GTK_MESSAGE_ERROR,
                                                            GTK_BUTTONS_CLOSE,
                                                            "Invalid file type. Please select an image.");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
        }
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}
