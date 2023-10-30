#ifndef SCAN_OPERATIONS_H
#define SCAN_OPERATIONS_H

#include <sane/sane.h>
#include <base.h>

typedef struct
{
    int width;
    int height;
    int depth;
    int format;
} ScanImageInfo;

typedef enum
{
    JOB_NOT_STARTED,
    JOB_RUNNING,
    JOB_COMPLETED,
    JOB_CANCELLED
} ScanJobStatus;

typedef struct
{
    ScanJobStatus status;
    SANE_Handle device_handle;
    ScanImageInfo image_info;
} ScanJob;

extern ScanJob global_scan_job;

SANE_Status initiate_scan_job(const char *device_name, const char *output_filename);
int cancel_scan_job();
int get_scan_image_info(struct MHD_Connection *connection);

#endif // SCAN_OPERATIONS_H
