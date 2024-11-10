#include <opencv2/opencv.hpp>
#include <gtk/gtk.h>
#include <iostream>
#include <vector>
#include <sys/stat.h> 
#include <cstring> 
#include <sndfile.h>
#include <algorithm>


#define Median_Window_Size_Image 17

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

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
void apply_median_filter(vector<float> &samples, int filter_size) {
    int half_size = filter_size / 2;
    vector<float> filtered(samples.size());

    for (size_t i = 0; i < samples.size(); ++i) {
       vector<float> window;
        for (int j = -half_size; j <= half_size; ++j) {
            if (i + j >= 0 && i + j < samples.size()) {
                window.push_back(samples[i + j]);
            }
        }
        sort(window.begin(), window.end());
        filtered[i] = window[window.size() / 2];
    }

    samples = filtered;
}

// Function to apply median filtering to audio and save the result
void apply_median_filter_to_audio(const char *filepath) {
    SF_INFO sfinfo;
    SNDFILE *infile = sf_open(filepath, SFM_READ, &sfinfo);
    if (!infile) {
        printf("Error: Unable to open file %s\n", filepath);
        return;
    }

    // Read audio samples
    vector<float> samples(sfinfo.frames * sfinfo.channels);
    sf_read_float(infile, samples.data(), samples.size());
    sf_close(infile);

    // Apply median filtering
    int filter_size = Median_Window_Size_Image; 
    apply_median_filter(samples, filter_size);

    // Create the directory if it doesn't exist
    mkdir("median_filtered_audios", 0777);

    // Save the filtered audio with the original filename
    string filename(filepath);
    string output_filename = "median_filtered_audios/" + filename.substr(filename.find_last_of("/") + 1);
    SNDFILE *outfile = sf_open(output_filename.c_str(), SFM_WRITE, &sfinfo);
    if (!outfile) {
        printf("Error: Unable to save file %s\n", output_filename.c_str());
        return;
    }
    sf_write_float(outfile, samples.data(), samples.size());
    sf_close(outfile);

    // Show popup notification
    string message = "Filtering done. The result is saved in 'median_filtered_audios'.";
    show_popup(message.c_str());
     cout << "Filtered audio saved in 'median_filtered_audios'." << endl;
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
            apply_median_filter_to_audio(filename); 
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


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------

void on_1d_array_input_save(GtkDialog *dialog, gint response_id, gpointer entry) {
    if (response_id == GTK_RESPONSE_ACCEPT) {
        const gchar *input = gtk_entry_get_text(GTK_ENTRY(entry));

        // Create the directory if it doesn't exist
        g_mkdir_with_parents("median_filtered_array", 0755);

        // Save the 1D array to a file
        FILE *file = fopen("median_filtered_array/array_1d.txt", "w");
        if (file != NULL) {
            fprintf(file, "%s\n", input);
            fclose(file);

            GtkWidget *success_dialog = gtk_message_dialog_new(NULL,
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_INFO,
                GTK_BUTTONS_OK,
                "1D array saved to median_filtered_array/array_1d.txt.");
            gtk_dialog_run(GTK_DIALOG(success_dialog));
            gtk_widget_destroy(success_dialog);
        }
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

void on_2d_array_input_save(GtkDialog *dialog, gint response_id, gpointer entry) {
    if (response_id == GTK_RESPONSE_ACCEPT) {
        const gchar *input = gtk_entry_get_text(GTK_ENTRY(entry));

        // Create the directory if it doesn't exist
        g_mkdir_with_parents("median_filtered_array", 0755);

        // Save the 2D array to a file
        FILE *file = fopen("median_filtered_array/array_2d.txt", "w");
        if (file != NULL) {
            fprintf(file, "%s\n", input);
            fclose(file);

            GtkWidget *success_dialog = gtk_message_dialog_new(NULL,
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_INFO,
                GTK_BUTTONS_OK,
                "2D array saved to median_filtered_array/array_2d.txt.");
            gtk_dialog_run(GTK_DIALOG(success_dialog));
            gtk_widget_destroy(success_dialog);
        }
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}



void on_1d_array_selected(GtkWidget *widget, gpointer dialog) {
    gtk_widget_destroy(GTK_WIDGET(dialog));

    // Create a new window to ask for 1D input
    GtkWidget *input_window, *content_area, *label, *entry, *save_button;
    input_window = gtk_dialog_new_with_buttons("1D Array Input",
                                               NULL,
                                              (GtkDialogFlags)(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
                                               "_Save", GTK_RESPONSE_ACCEPT,
                                               "_Cancel", GTK_RESPONSE_CANCEL,
                                               NULL);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(input_window));
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    label = gtk_label_new("Please enter the 1D array elements separated by spaces:");
    gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);

    entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box), entry, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(content_area), box);

    g_signal_connect(input_window, "response", G_CALLBACK(on_1d_array_input_save), entry);

    gtk_widget_show_all(input_window);
}

void on_2d_array_selected(GtkWidget *widget, gpointer dialog) {
    gtk_widget_destroy(GTK_WIDGET(dialog));

    // Create a new window to ask for 2D input
    GtkWidget *input_window, *content_area, *label, *entry, *save_button;
    input_window = gtk_dialog_new_with_buttons("2D Array Input",
                                               NULL,
                                               (GtkDialogFlags)(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
                                               "_Save", GTK_RESPONSE_ACCEPT,
                                               "_Cancel", GTK_RESPONSE_CANCEL,
                                               NULL);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(input_window));
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    label = gtk_label_new("Please enter the 2D array elements separated by spaces, with each row separated by commas:");
    gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);

    entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box), entry, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(content_area), box);

    g_signal_connect(input_window, "response", G_CALLBACK(on_2d_array_input_save), entry);

    gtk_widget_show_all(input_window);
}



void on_manual_input_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkWidget *content_area;
    GtkWidget *label;
    GtkWidget *box;
    GtkWidget *one_d_button, *two_d_button;

    dialog = gtk_dialog_new_with_buttons("Select Array Type",
                                         GTK_WINDOW(data),
                                         (GtkDialogFlags)(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         NULL);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    label = gtk_label_new("Please choose the array type:");
    gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);

    one_d_button = gtk_button_new_with_label("1D Array");
    two_d_button = gtk_button_new_with_label("2D Array");

    gtk_box_pack_start(GTK_BOX(box), one_d_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), two_d_button, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(content_area), box);

    g_signal_connect(one_d_button, "clicked", G_CALLBACK(on_1d_array_selected), dialog);
    g_signal_connect(two_d_button, "clicked", G_CALLBACK(on_2d_array_selected), dialog);

    gtk_widget_show_all(dialog);
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------


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
    g_signal_connect(manual_button, "clicked", G_CALLBACK(on_manual_input_clicked), window);

    // Connect signal to close the window
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

