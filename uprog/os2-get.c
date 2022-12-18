#include <stdio.h>
#include <string.h>

#define PROCFS_NAME "/proc/labOS2"
#define MODE_PID 'p'
#define MODE_DEVICE 'd'
#define BUF_SIZE 100

// ------------------------------------------------------------
// FORMING QUERY
// ------------------------------------------------------------

int get_mode(char *flag)
{
    if (flag[0] != '-')
        return -1;

    switch (flag[1])
    {
    case 'p':
        return MODE_PID;
    case 'd':
        return MODE_DEVICE;
    default:
        return -1;
    }
}

int form_input_string(int mode, char *buf, char *content)
{
    buf[0] = (char)mode;
    buf[1] = 0;

    if (strlen(content) > BUF_SIZE - 1)
        return -1;

    strcat(buf, content);
    return 0;
}

// ------------------------------------------------------------
// I/O
// ------------------------------------------------------------

int write_to_file(char *buf)
{
    FILE *file;
    file = fopen(PROCFS_NAME, "w");
    if (!file)
        return -1;

    size_t message_size = strlen(buf);
    if (fprintf(file, "%s", buf) != message_size)
        return -1;

    fclose(file);
    return 0;
}

int print_file(void)
{
    FILE *file;
    file = fopen(PROCFS_NAME, "r");
    if (!file)
        return -1;

    int c;
    while ((c = fgetc(file)) != EOF)
        printf("%c", c);

    if (!feof(file))
        return -1;

    return 0;
}

// ------------------------------------------------------------
// MAIN
// ------------------------------------------------------------

void print_usage(void)
{
    fprintf(stderr, "Usage:\n \
    To get info about task_struct:\n \
    os2-get -p <PID>\n \
    To get info about net_device:\n \
    os2-get -d <name>\n");
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        print_usage();
        return -1;
    }

    int mode;
    mode = get_mode(argv[1]);
    if (mode == -1)
    {
        print_usage();
        return -1;
    }

    char buf[BUF_SIZE];
    if (form_input_string(mode, buf, argv[2]) != 0)
    {
        fprintf(stderr, "Second argument is too long!\n");
        return -1;
    }

    if (write_to_file(buf) != 0)
    {
        fprintf(stderr, "Couldn't write to procfs. Do you have permissions? Is the labOS2 module running?\n");
        return -1;
    }

    if (print_file() != 0)
    {
        fprintf(stderr, "Couldn't read from procfs. Do you have permissions? Is the labOS2 module running?\n");
        return -1;
    }

    return 0;
}