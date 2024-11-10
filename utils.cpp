#include "utils.h"


void show_popup(const char* message) {
    GtkWidget *dialog = gtk_message_dialog_new(NULL,
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

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
