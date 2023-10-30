#include "include/base.h"
#include "include/http_handlers.h"
#include "include/scan_operations.h"
#include "include/utils.h"

const char *get_first_scanner_name()
{
    const SANE_Device **device_list;
    SANE_Status status = sane_get_devices(&device_list, SANE_FALSE);
    if (status != SANE_STATUS_GOOD || !device_list[0])
    {
        return NULL;
    }
    return device_list[0]->name;
}

SANE_Status scan_image(const char *scanner_name, const char *output_filename)
{
    SANE_Status status;
    SANE_Handle device_handle;
    const SANE_Option_Descriptor *opt;
    SANE_Int num_options;
    SANE_Parameters params;
    FILE *output_file;

    status = sane_init(NULL, NULL);
    if (status != SANE_STATUS_GOOD)
    {
        return status;
    }

    status = sane_open(scanner_name, &device_handle);
    if (status != SANE_STATUS_GOOD)
    {
        sane_exit();
        return status;
    }

    sane_control_option(device_handle, 0, SANE_ACTION_GET_VALUE, &num_options, NULL);

    for (int i = 0; i < num_options; i++)
    {
        opt = sane_get_option_descriptor(device_handle, i);
    }

    status = sane_start(device_handle);
    if (status != SANE_STATUS_GOOD)
    {
        sane_close(device_handle);
        sane_exit();
        return status;
    }

    status = sane_get_parameters(device_handle, &params);
    if (status != SANE_STATUS_GOOD)
    {
        sane_close(device_handle);
        sane_exit();
        return status;
    }

    output_file = fopen(output_filename, "wb");
    if (!output_file)
    {
        perror("Failed to open output file");
        sane_close(device_handle);
        sane_exit();
        return SANE_STATUS_NO_MEM;
    }

    {
        SANE_Byte buffer[1024];
        int length;

        while ((length = sane_read(device_handle, buffer, sizeof(buffer), &status)) > 0)
        {
            fwrite(buffer, 1, length, output_file);
        }
    }

    fclose(output_file);

    sane_close(device_handle);
    sane_exit();

    return status;
}

int main()
{
    printf("Choose an option:\n");
    printf("1. Directly connect to a scanner\n");
    printf("2. Run as a server\n");
    printf("Enter your choice (1/2): ");

    int choice;
    scanf("%d", &choice);

    if (choice == 1)
    {
        printf("Connecting directly to the scanner...\n");

        const char *scanner_name = get_first_scanner_name();
        if (!scanner_name)
        {
            printf("No scanners detected.\n");
            return 1;
        }
        printf("Using scanner: %s\n", scanner_name);

        const char *output_filename = "output_file.pnm";

        SANE_Status status = scan_image(scanner_name, output_filename);
        if (status == SANE_STATUS_GOOD)
        {
            printf("Scan completed successfully!\n");
        }
        else
        {
            printf("Failed to scan: %s\n", sane_strstatus(status));
        }
    }
    else if (choice == 2)
    {
        printf("Running as a server on port %d...\n", PORT);

        struct MHD_Daemon *daemon;
        daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                                  &answer_to_connection, NULL, MHD_OPTION_END);
        if (daemon == NULL)
        {
            fprintf(stderr, "Error starting the server.\n");
            return 1;
        }

        printf("Server is running. Press [Enter] to stop...\n");
        getchar();
        getchar();

        MHD_stop_daemon(daemon);
    }
    else
    {
        printf("Invalid choice.\n");
    }

    return 0;
}