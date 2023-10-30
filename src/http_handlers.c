#include "../include/http_handlers.h"
#include "../include/base.h"
#include "../include/scan_operations.h"

int handle_scan_job_post(struct MHD_Connection *connection)
{
    const char *output_filename = "scanned_output.pnm";
    const SANE_Device **device_list;
    SANE_Status status;

    status = sane_get_devices(&device_list, SANE_FALSE);
    if (status != SANE_STATUS_GOOD)
    {
        return MHD_HTTP_INTERNAL_SERVER_ERROR;
    }

    if (device_list[0] == NULL)
    {
        return MHD_HTTP_SERVICE_UNAVAILABLE;
    }

    status = initiate_scan_job(device_list[0]->name, output_filename);
    if (status != SANE_STATUS_GOOD)
    {
        return MHD_HTTP_INTERNAL_SERVER_ERROR;
    }

    global_scan_job.status = JOB_COMPLETED;

    return MHD_HTTP_OK;
}
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls)
{
    if (strcmp(method, "GET") == 0 && !strstr(url, "ScanImageInfo"))
    {
        char *xml_response = get_scanner_capabilities();
        if (xml_response == NULL)
        {
            const char *error_msg = "Internal Server Error";
            struct MHD_Response *response = MHD_create_response_from_buffer(strlen(error_msg), (void *)error_msg, MHD_RESPMEM_PERSISTENT);
            int ret = MHD_queue_response(connection, 500, response);
            MHD_destroy_response(response);
            return ret;
        }
        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(xml_response), (void *)xml_response, MHD_RESPMEM_MUST_FREE);
        MHD_add_response_header(response, "Content-Type", "application/xml");
        int ret = MHD_queue_response(connection, 200, response);
        MHD_destroy_response(response);
        free(xml_response);
        return ret;
    }
    else if (strcmp(method, "GET") == 0 && strstr(url, "ScanImageInfo"))
    {
        return get_scan_image_info(connection);
    }
    else if (strcmp(method, "PUT") == 0)
    {
        struct MHD_Response *response;
        int ret;
        struct ConnectionInfo *con_info = *con_cls;

        if (*upload_data_size != 0)
        {
            con_info->message = realloc(con_info->message, con_info->size + *upload_data_size + 1);
            strncpy(con_info->message + con_info->size, upload_data, *upload_data_size);
            con_info->size += *upload_data_size;
            *upload_data_size = 0;
            return MHD_YES;
        }

        char response_xml[] = "<scan:ScanBufferInfo>YOUR_RESPONSE_XML_HERE</scan:ScanBufferInfo>";
        response = MHD_create_response_from_buffer(strlen(response_xml), (void *)response_xml, MHD_RESPMEM_PERSISTENT);
        MHD_add_response_header(response, "Content-Type", "application/xml");
        ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);
        free(con_info->message);
        free(con_info);
        return ret;
    }
    else if (strcmp(method, "POST") == 0 && strcmp(url, "/ScanJobs") == 0)
    {
        int ret = handle_scan_job_post(connection);

        struct MHD_Response *response = MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_PERSISTENT);
        return MHD_queue_response(connection, ret, response);
    }
    else if (strcmp(method, "DELETE") == 0)
    {
        int ret = cancel_scan_job();

        struct MHD_Response *response = MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_PERSISTENT);
        return MHD_queue_response(connection, ret, response);
    }
    else
    {
        struct MHD_Response *response = MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_PERSISTENT);
        int ret = MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
        MHD_destroy_response(response);
        return ret;
    }
}
