#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void copy_files(const char *src_file, const char *dst_file, struct stat src_stat)
{
    int src_fd = open(src_file, O_RDONLY);
    if (src_fd == -1)
    {
        close(src_fd);
        error("Error al abrir el archivo fuente");
    }

    off_t file_size = src_stat.st_size;

    int dst_fd = open(dst_file, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (dst_fd == -1)
    {
        close(src_fd);
        close(dst_fd);
        error("Error al abrir el archivo destino");
    }

    // se utiliza ftruncate para establecer el tamaño del archivo destino al mismo tamaño que el archivo fuente
    if (ftruncate(dst_fd, file_size) == -1)
    {
        close(src_fd);
        close(dst_fd);
        error("Error al truncar el archivo destino");
    }

    void *src_addr = mmap(NULL, file_size, PROT_READ, MAP_SHARED, src_fd, 0);
    if (src_addr == MAP_FAILED)
    {
        close(src_fd);
        close(dst_fd);
        error("Error al mapear el archivo fuente a memoria");
    }

    void *dst_addr = mmap(NULL, file_size, PROT_WRITE, MAP_SHARED, dst_fd, 0);

    close(src_fd);
    close(dst_fd);
    if (dst_addr == MAP_FAILED)
    {
        error("Error al mapear el archivo destino a memoria");
    }

    memcpy(dst_addr, src_addr, file_size);

    if (munmap(src_addr, file_size) == -1)
    {
        error("Error al liberar el mapeo del archivo fuente");
    }

    if (munmap(dst_addr, file_size) == -1)
    {
        error("Error al liberar el mapeo del archivo destino");
    }

    printf("Archivo copiado exitosamente: %s -> %s\n", src_file, dst_file);
}

int previous_verification(const char *src_file, const char *dst_file, struct stat *src_stat)
{
    // Verificar que el archivo fuente existe

    if (stat(src_file, src_stat) == -1)
    {
        fprintf(stderr, "Error al obtener los atributos del archivo fuente");
    }

    // Verificar que el archivo fuente es regular
    if (!S_ISREG(src_stat->st_mode))
    {
        fprintf(stderr, "El archivo fuente no es un archivo regular\n");
        return 1;
    }

    // Verificar que el archivo destino no existe

    struct stat dst_stat;
    if (stat(dst_file, &dst_stat) != -1)
    {
        fprintf(stderr, "El archivo destino ya existe\n");
        return 1;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: <src_file> <dst_file>\n");
        return 1;
    }

    const char *src_file = argv[1];
    const char *dst_file = argv[2];

    struct stat src_stat;
    if (previous_verification(src_file, dst_file, &src_stat) == 1)
    {
        return 1;
    }

    copy_files(src_file, dst_file, src_stat);

    return 0;
}
