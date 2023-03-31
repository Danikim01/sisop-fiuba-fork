#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>

void leer_directorios(DIR* directory, char* path,char* cadena_a_buscar,char *(*funcion_a_comparar)(const char *, const char *)){
    struct dirent *entry;

    while ((entry = readdir(directory)) != NULL){
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
            continue;
        }

        char entry_path[PATH_MAX];
        strcpy(entry_path, path);
		strcat(entry_path, "/");
		strcat(entry_path, entry->d_name);

        if (entry->d_type == DT_DIR){
            if ((*funcion_a_comparar)(entry_path, cadena_a_buscar)) {
                    printf("%s\n",entry_path);
	        }  
            int subdirectory = openat(dirfd(directory), entry->d_name, O_DIRECTORY);
            if (subdirectory == -1){
                perror("error con el openat");
                exit(-1);
            }

            DIR* subdirectory_pointer = fdopendir(subdirectory);
            if (subdirectory_pointer == NULL){
                perror("error con el fdopendir");
                exit(-1);
            }

            leer_directorios(subdirectory_pointer,entry_path,cadena_a_buscar,funcion_a_comparar);

            if (closedir(subdirectory_pointer) == -1){
                perror("error con el closedir");
                exit(-1);
            }
        }
        else if(entry->d_type == DT_REG){
                if ((*funcion_a_comparar)(entry->d_name, cadena_a_buscar)) {
                    printf("%s\n",entry_path);
	            }      
        }
    }
}


int main(int argc, char *argv[]){
    char path_actual[]  = ".";
    DIR* directory = opendir(path_actual);
    if (directory == NULL){
        perror("error con el opendir");
        exit(-1);
    }
    
    if (argc==2){
        leer_directorios(directory,path_actual,argv[1],strstr);
    }else if(argc == 3 && strcmp(argv[1],"-i")==0){
        leer_directorios(directory,path_actual,argv[2],strcasestr);
    }

    closedir(directory);
    
    return 0;

}