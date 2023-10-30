#ifndef UTILS_H
#define UTILS_H

#include <base.h>
#include <sane/sane.h>

void append_to_xml(char *buffer, const char *data);
void handle_option_constraints(char *buffer, const SANE_Option_Descriptor *opt);
void append_option_capabilities(char *buffer, const SANE_Option_Descriptor *opt);
void append_option_units(char *buffer, const SANE_Option_Descriptor *opt);
char *get_scanner_capabilities();

#endif // UTILS_H
