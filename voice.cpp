
#include "voice.h"


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
    int filter_size = Median_Window_Size_Voice; 
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


