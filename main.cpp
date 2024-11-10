#include <opencv2/opencv.hpp>
#include <gtk/gtk.h> 

#include "image.h"
#include "voice.h"

using namespace cv;
using namespace std;


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
    //manual_button = gtk_button_new_with_label("Manual Input (Array)");

    // Attach buttons to the grid
    gtk_grid_attach(GTK_GRID(grid), image_button, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), audio_button, 0, 1, 1, 1);
    //gtk_grid_attach(GTK_GRID(grid), manual_button, 0, 2, 1, 1);

    // Connect signals to buttons
    g_signal_connect(image_button, "clicked", G_CALLBACK(on_image_input_clicked), window);
    g_signal_connect(audio_button, "clicked", G_CALLBACK(on_audio_input_clicked), window);
    //g_signal_connect(manual_button, "clicked", G_CALLBACK(on_manual_input_clicked), window);

    // Connect signal to close the window
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

