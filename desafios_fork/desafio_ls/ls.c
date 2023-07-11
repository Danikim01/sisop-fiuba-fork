#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <limits.h>
#include <inttypes.h>

typedef struct
{
    char type;
    char permissions[10];
    char owner_name[32];
    char group_name[32];
    char name[256];
} EntryInfo;

int compare_entries(const void *a, const void *b)
{
    const EntryInfo *entry1 = (const EntryInfo *)a;
    const EntryInfo *entry2 = (const EntryInfo *)b;
    return strcmp(entry1->name, entry2->name);
}

void set_permissions(char permissions[10], struct stat entry_stat)
{
    permissions[0] = (entry_stat.st_mode & S_IRUSR) ? 'r' : '-';
    permissions[1] = (entry_stat.st_mode & S_IWUSR) ? 'w' : '-';
    permissions[2] = (entry_stat.st_mode & S_IXUSR) ? 'x' : '-';
    permissions[3] = (entry_stat.st_mode & S_IRGRP) ? 'r' : '-';
    permissions[4] = (entry_stat.st_mode & S_IWGRP) ? 'w' : '-';
    permissions[5] = (entry_stat.st_mode & S_IXGRP) ? 'x' : '-';
    permissions[6] = (entry_stat.st_mode & S_IROTH) ? 'r' : '-';
    permissions[7] = (entry_stat.st_mode & S_IWOTH) ? 'w' : '-';
    permissions[8] = (entry_stat.st_mode & S_IXOTH) ? 'x' : '-';
    permissions[9] = '\0';
}

void set_file_type(char *type, struct stat entry_stat)
{
    if (S_ISDIR(entry_stat.st_mode))
    {
        (*type) = 'd';
    }
    else if (S_ISLNK(entry_stat.st_mode))
    {
        (*type) = 'l';
    }
    else
    {
        (*type) = '-';
    }
}

void set_user_and_group(struct stat entry_stat, char *owner_name, char *group_name)
{
    // Obtener el nombre del usuario propietario del archivo
    struct passwd *pw = getpwuid(entry_stat.st_uid);
    strncpy(owner_name, (pw != NULL) ? pw->pw_name : "", 32);

    // Obtener el nombre del grupo propietario del archivo
    struct group *gr = getgrgid(entry_stat.st_gid);
    strncpy(group_name, (gr != NULL) ? gr->gr_name : "", 32);
}

void print_symbolic_link(char type, const char *path)
{
    if (type == 'l')
    {
        char target_path[PATH_MAX];
        ssize_t len = readlink(path, target_path, sizeof(target_path) - 1);
        if (len != -1)
        {
            target_path[len] = '\0';
            printf(" -> %s", target_path);
        }
    }
}

void print_entry_info(const EntryInfo *entry)
{
    // Obtener el UID del propietario del archivo
    struct stat entry_stat;
    if (lstat(entry->name, &entry_stat) == -1)
    {
        printf("Error al obtener información de entrada: %s\n", entry->name);
        return;
    }
    uid_t owner_uid = entry_stat.st_uid;
    printf("%c%s %s uid: %u %s %s", entry->type, entry->permissions, entry->owner_name, owner_uid, entry->group_name, entry->name);
    print_symbolic_link(entry->type, entry->name);
    printf("\n");
}

int main(int argc, char *argv[])
{
    const char *dir_path = "."; // Directorio actual por defecto

    if (argc > 1)
    {
        dir_path = argv[1]; // Usar el directorio proporcionado como argumento
    }

    DIR *dir = opendir(dir_path);
    if (dir == NULL)
    {
        printf("No se pudo abrir el directorio: %s\n", dir_path);
        return 1;
    }

    struct dirent *entry;
    EntryInfo entries[1000];
    int entry_count = 0;

    while ((entry = readdir(dir)) != NULL)
    {
        char entry_path[PATH_MAX];
        snprintf(entry_path, sizeof(entry_path), "%s/%s", dir_path, entry->d_name);

        struct stat entry_stat;
        if (lstat(entry_path, &entry_stat) == -1)
        {
            printf("Error al obtener información de entrada: %s\n", entry_path);
            continue;
        }

        EntryInfo *current_entry = &entries[entry_count];
        set_file_type(&current_entry->type, entry_stat);
        set_permissions(current_entry->permissions, entry_stat);
        set_user_and_group(entry_stat, current_entry->owner_name, current_entry->group_name);
        strncpy(current_entry->name, entry->d_name, sizeof(current_entry->name));

        entry_count++;
    }

    closedir(dir);
    // Ordenar las entradas por nombre
    qsort(entries, entry_count, sizeof(EntryInfo), compare_entries);

    for (int i = 0; i < entry_count; i++)
    {
        print_entry_info(&entries[i]);
    }

    return 0;
}