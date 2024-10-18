#include <opencv2/opencv.hpp>
#include <gtk/gtk.h>
#include <iostream>
#include <vector>
#include <sys/stat.h> 
#include <cstring> 

#define Median_Window_Size_Image 3

using namespace cv;
using namespace std;


void show_popup(const char* message) {
    GtkWidget *dialog = gtk_message_dialog_new(NULL,
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}


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


// Function to check if a file has a valid extension
int check_file_extension(const char* filename, const char** valid_extensions, int num_extensions) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return 0;
    for (int i = 0; i < num_extensions; i++) {
        if (strcmp(dot + 1, valid_extensions[i]) == 0) {
            return 1;
        }
    }
    return 0;
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
    imwrite("output_image.png", image);
    string message = "Image uploaded and filtered. Result saved in 'median_filtered_images'.";
    show_popup(message.c_str());
    cout << "Filtered image saved as output_image.png" << endl;
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

// Callback for Audio Input Button
void on_audio_input_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    const char *valid_extensions[] = {"wav", "flac", "ogg", "mp3"};

    dialog = gtk_file_chooser_dialog_new("Select an Audio File",
                                         GTK_WINDOW(data),
                                         action,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_ACCEPT,
                                         NULL);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_mime_type(filter, "audio/wav");
    gtk_file_filter_add_mime_type(filter, "audio/flac");
    gtk_file_filter_add_mime_type(filter, "audio/ogg");
    gtk_file_filter_add_mime_type(filter, "audio/mp3");
    gtk_file_filter_set_name(filter, "Audio Files");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);

        // Validate the file extension
        if (check_file_extension(filename, valid_extensions, 4)) {
            printf("Selected Audio: %s\n", filename);
        } else {
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                            GTK_DIALOG_DESTROY_WITH_PARENT,
                                                            GTK_MESSAGE_ERROR,
                                                            GTK_BUTTONS_CLOSE,
                                                            "Invalid file type. Please select an audio file.");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
        }
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}
/*
// Callback for Manual Input Button
void on_manual_input_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog, *content_area, *entry;
    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    // Create a dialog for manual input
    dialog = gtk_dialog_new_with_buttons("Manual Input",
                                         GTK_WINDOW(data),
                                         flags,
                                         "_OK", GTK_RESPONSE_ACCEPT,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         NULL);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    entry = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(content_area), entry);

    gtk_widget_show_all(dialog);

    gint result = gtk_dialog_run(GTK_DIALOG(dialog));
    if (result == GTK_RESPONSE_ACCEPT) {
        const char *input_text = gtk_entry_get_text(GTK_ENTRY(entry));
        printf("Manual Input: %s\n", input_text);
    }

    gtk_widget_destroy(dialog);
}
*/
// Main function to set up the GUI
int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *image_button;
    GtkWidget *audio_button;
    GtkWidget *manual_button;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Input Selection");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Create buttons
    image_button = gtk_button_new_with_label("Select Image");
    audio_button = gtk_button_new_with_label("Select Audio");
    manual_button = gtk_button_new_with_label("Manual Input (Array)");

    // Attach buttons to the grid
    gtk_grid_attach(GTK_GRID(grid), image_button, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), audio_button, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), manual_button, 0, 2, 1, 1);

    // Connect signals to buttons
    g_signal_connect(image_button, "clicked", G_CALLBACK(on_image_input_clicked), window);
    g_signal_connect(audio_button, "clicked", G_CALLBACK(on_audio_input_clicked), window);
//    g_signal_connect(manual_button, "clicked", G_CALLBACK(on_manual_input_clicked), window);

    // Connect signal to close the window
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

