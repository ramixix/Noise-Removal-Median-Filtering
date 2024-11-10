#ifndef UTILS_H
#define UTILS_H

#include <cstring>
#include <gtk/gtk.h>

void show_popup(const char* message);
int check_file_extension(const char* filename, const char** valid_extensions, int num_extensions);

#endif // UTILS_H
