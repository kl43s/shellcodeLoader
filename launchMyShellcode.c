#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int main(int argc, char** argv) {
    FILE* fichier;
    char* donnees;
    long taille_fichier;

    if (argc != 3) {
        printf("[ Info ] %s <filename> <offset> \n", argv[0]);
        return 1;
    }

    errno_t err;
    err = fopen_s(&fichier, argv[1], "rb");
    if (err != 0) {
        return 1;
    }

    char* end;
    int offset = strtol(argv[2], &end, 16);
    if (*end != '\0') {
        printf("[ Info ] '%s' n'est pas un nombre hexadécimal \n", argv[2]);
        return 1;
    }

    fseek(fichier, 0, SEEK_END);
    taille_fichier = ftell(fichier);
    rewind(fichier);

    donnees = (char*)malloc(taille_fichier);
    if (donnees == NULL) {
        return 1;
    }

    size_t elements_lus = fread(donnees, 1, taille_fichier, fichier);
    if (elements_lus != taille_fichier) {
        return 1;
    }

    fclose(fichier);
    LPVOID nouvelle_memoire = VirtualAlloc(NULL, taille_fichier, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (nouvelle_memoire == NULL) {
        return 1;
    }

    memcpy(nouvelle_memoire, donnees, taille_fichier);
    void (*fonction_executable)() = (void (*)())((uintptr_t)nouvelle_memoire + offset);
    fonction_executable();

    VirtualFree(nouvelle_memoire, 0, MEM_RELEASE);
    free(donnees);
    return 0;
}
