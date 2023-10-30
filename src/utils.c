#include "../include/base.h"
#include "../include/utils.h"

void append_to_xml(char *buffer, const char *data)
{
    strcat(buffer, data);
}
void handle_option_constraints(char *buffer, const SANE_Option_Descriptor *opt)
{
    switch (opt->constraint_type)
    {
    case SANE_CONSTRAINT_RANGE:
        append_to_xml(buffer, "<constraint type=\"range\">");
        char tmp[256];
        snprintf(tmp, sizeof(tmp), "<min>%d</min>", opt->constraint.range->min);
        append_to_xml(buffer, tmp);
        snprintf(tmp, sizeof(tmp), "<max>%d</max>", opt->constraint.range->max);
        append_to_xml(buffer, tmp);
        snprintf(tmp, sizeof(tmp), "<quant>%d</quant>", opt->constraint.range->quant);
        append_to_xml(buffer, tmp);
        append_to_xml(buffer, "</constraint>");
        break;
    case SANE_CONSTRAINT_WORD_LIST:
    {
        int i;
        append_to_xml(buffer, "<constraint type=\"list\">");
        for (i = 1; i <= opt->constraint.word_list[0]; i++)
        {
            snprintf(tmp, sizeof(tmp), "<item>%d</item>", opt->constraint.word_list[i]);
            append_to_xml(buffer, tmp);
        }
        append_to_xml(buffer, "</constraint>");
        break;
    }
    case SANE_CONSTRAINT_STRING_LIST:
    {
        int i;
        append_to_xml(buffer, "<constraint type=\"string_list\">");
        for (i = 0; opt->constraint.string_list[i] != NULL; i++)
        {
            snprintf(tmp, sizeof(tmp), "<item>%s</item>", opt->constraint.string_list[i]);
            append_to_xml(buffer, tmp);
        }
        append_to_xml(buffer, "</constraint>");
        break;
    }
    }
}
void append_option_capabilities(char *buffer, const SANE_Option_Descriptor *opt)
{
    char tmp[256];
    snprintf(tmp, sizeof(tmp), "<capabilities>%d</capabilities>", opt->cap);
    append_to_xml(buffer, tmp);
}
void append_option_units(char *buffer, const SANE_Option_Descriptor *opt)
{
    char tmp[256];
    switch (opt->unit)
    {
    case SANE_UNIT_NONE:
        append_to_xml(buffer, "<unit>NONE</unit>");
        break;
    case SANE_UNIT_PIXEL:
        append_to_xml(buffer, "<unit>PIXEL</unit>");
        break;
    case SANE_UNIT_BIT:
        append_to_xml(buffer, "<unit>BIT</unit>");
        break;
    case SANE_UNIT_MM:
        append_to_xml(buffer, "<unit>MM</unit>");
        break;
    case SANE_UNIT_DPI:
        append_to_xml(buffer, "<unit>DPI</unit>");
        break;
    case SANE_UNIT_PERCENT:
        append_to_xml(buffer, "<unit>PERCENT</unit>");
        break;
    case SANE_UNIT_MICROSECOND:
        append_to_xml(buffer, "<unit>MICROSECOND</unit>");
        break;
    }
}
char *get_scanner_capabilities()
{
    SANE_Status status;
    SANE_Int version_code, num_options, i;
    const SANE_Device **device_list;
    SANE_Handle device_handle;
    char xml_output[8192] = "";

    status = sane_init(&version_code, NULL);
    if (status != SANE_STATUS_GOOD)
    {
        return NULL;
    }

    status = sane_get_devices(&device_list, SANE_FALSE);
    if (status != SANE_STATUS_GOOD)
    {
        sane_exit();
        return NULL;
    }

    if (device_list[0] == NULL)
    {
        sane_exit();
        return NULL;
    }

    status = sane_open(device_list[0]->name, &device_handle);
    if (status != SANE_STATUS_GOOD)
    {
        sane_exit();
        return NULL;
    }

    strcat(xml_output, "<scan:ScannerCapabilities>");
    strcat(xml_output, "<name>");
    strcat(xml_output, device_list[0]->name);
    strcat(xml_output, "</name>");
    strcat(xml_output, "<model>");
    strcat(xml_output, device_list[0]->model);
    strcat(xml_output, "</model>");
    strcat(xml_output, "<type>");
    strcat(xml_output, device_list[0]->type);
    strcat(xml_output, "</type>");

    num_options = sane_get_option_descriptor(device_handle, 0)->size;

    SANE_Int group_code = -1;
    for (i = 1; i < num_options; i++)
    {
        const SANE_Option_Descriptor *opt = sane_get_option_descriptor(device_handle, i);
        if (opt)
        {
            if (opt->type == SANE_TYPE_GROUP)
            {
                if (group_code != -1)
                {
                    strcat(xml_output, "</group>");
                }
                group_code = i;
                strcat(xml_output, "<group>");
                strcat(xml_output, "<title>");
                strcat(xml_output, opt->title);
                strcat(xml_output, "</title>");
                continue;
            }

            strcat(xml_output, "<option>");
            strcat(xml_output, "<name>");
            strcat(xml_output, opt->name);
            strcat(xml_output, "</name>");
            strcat(xml_output, "<type>");
            strcat(xml_output, sane_strstatus(opt->type));
            strcat(xml_output, "</type>");
            strcat(xml_output, "</option>");
        }
    }
    if (group_code != -1)
    {
        strcat(xml_output, "</group>");
    }

    strcat(xml_output, "</scan:ScannerCapabilities>");

    sane_close(device_handle);
    sane_exit();

    char *result = malloc(strlen(xml_output) + 1);
    strcpy(result, xml_output);
    return result;
}