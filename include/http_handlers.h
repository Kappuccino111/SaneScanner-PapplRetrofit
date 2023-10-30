#ifndef HTTP_HANDLERS_H
#define HTTP_HANDLERS_H

#include <microhttpd.h>
#include <base.h>

#define PORT 8080

struct ConnectionInfo
{
    char *message;
    size_t size;
};
int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **ptr);

int handle_scan_job_post(struct MHD_Connection *connection);

#endif // HTTP_HANDLERS_H
