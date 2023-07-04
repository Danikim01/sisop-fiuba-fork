#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

void print_process_info(const char *pid);

int main()
{
    DIR *proc_dir = opendir("/proc");
    if (proc_dir == NULL)
    {
        perror("No se pudo abrir el directorio /proc");
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(proc_dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        if (entry->d_type == DT_DIR)
        {
            print_process_info(entry->d_name);
        }
    }

    closedir(proc_dir);
    return 0;
}

void print_process_info(const char *pid)
{
    // Abrir el archivo comm para obtener el nombre del comando del proceso
    char comm_path[256];
    snprintf(comm_path, sizeof(comm_path), "/proc/%s/comm", pid);
    FILE *comm_file = fopen(comm_path, "r");
    if (comm_file)
    {
        char comm[1024];
        if (fgets(comm, sizeof(comm), comm_file))
        {
            // Mostrar el PID y el nombre del comando del proceso
            printf("PID: %s, Comand: %s", pid, comm);
        }
        fclose(comm_file);
    }
}