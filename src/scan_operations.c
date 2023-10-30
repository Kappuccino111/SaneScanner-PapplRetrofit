#include "../include/base.h"
#include "../include/scan_operations.h"
#include "../include/http_handlers.h"

ScanJob global_scan_job = {
    .status = JOB_NOT_STARTED,
    .device_handle = NULL};

SANE_Status initiate_scan_job(const char *device_name, const char *output_filename)
{
    SANE_Status status;
    SANE_Handle device_handle;
    SANE_Int num_bytes;
    SANE_Byte buffer[1024];
    FILE *fp;

    status = sane_open(device_name, &device_handle);
    if (status != SANE_STATUS_GOOD)
    {
        return status;
    }

    status = sane_start(device_handle);
    if (status != SANE_STATUS_GOOD)
    {
        sane_close(device_handle);
        return status;
    }

    fp = fopen(output_filename, "wb");
    if (!fp)
    {
        sane_cancel(device_handle);
        sane_close(device_handle);
        return SANE_STATUS_INVAL;
    }

    do
    {
        status = sane_read(device_handle, buffer, sizeof(buffer), &num_bytes);

        if (status == SANE_STATUS_GOOD || status == SANE_STATUS_EOF)
        {
            if (num_bytes > 0)
            {
                fwrite(buffer, 1, num_bytes, fp);
            }
        }
        else
        {
            fclose(fp);
            sane_cancel(device_handle);
            sane_close(device_handle);
            return status;
        }
    } while (status != SANE_STATUS_EOF);

    fclose(fp);
    sane_close(device_handle);
    return SANE_STATUS_GOOD;
}

int cancel_scan_job()
{
    if (global_scan_job.status == JOB_RUNNING && global_scan_job.device_handle)
    {
        sane_cancel(global_scan_job.device_handle);
        sane_close(global_scan_job.device_handle);
        global_scan_job.device_handle = NULL;
        global_scan_job.status = JOB_CANCELLED;
        return MHD_HTTP_OK;
    }
    else if (global_scan_job.status == JOB_NOT_STARTED)
    {
        return MHD_HTTP_NOT_FOUND;
    }
    else if (global_scan_job.status == JOB_COMPLETED)
    {
        return MHD_HTTP_GONE;
    }
    else
    {
        return MHD_HTTP_INTERNAL_SERVER_ERROR;
    }
}
int get_scan_image_info(struct MHD_Connection *connection)
{
    if (global_scan_job.status == JOB_NOT_STARTED)
    {
        return MHD_HTTP_NOT_FOUND;
    }
    else if (global_scan_job.status == JOB_RUNNING)
    {
        return MHD_HTTP_SERVICE_UNAVAILABLE;
    }
    else if (global_scan_job.status == JOB_CANCELLED)
    {
        return MHD_HTTP_GONE;
    }
    else if (global_scan_job.status == JOB_COMPLETED)
    {
        char xml_payload[1024];
        sprintf(xml_payload,
                "<scan:ScanImageInfo>"
                "<Width>%d</Width>"
                "<Height>%d</Height>"
                "<Depth>%d</Depth>"
                "<Format>%d</Format>"
                "</scan:ScanImageInfo>",
                global_scan_job.image_info.width,
                global_scan_job.image_info.height,
                global_scan_job.image_info.depth,
                global_scan_job.image_info.format);

        struct MHD_Response *response = MHD_create_response_from_buffer(
            strlen(xml_payload),
            (void *)xml_payload,
            MHD_RESPMEM_MUST_COPY);

        MHD_add_response_header(response, "Content-Type", "application/xml");
        return MHD_queue_response(connection, MHD_HTTP_OK, response);
    }
    else
    {
        return MHD_HTTP_INTERNAL_SERVER_ERROR;
    }
}