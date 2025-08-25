#include "loadShader.h"

char* loadShader(const char* filename){
    // Datei öffnen
    FILE * file=fopen(filename, "r");
    if(file==NULL){
        printf("Error opening file %s\n", filename);
        return NULL;
    }
    // länge der Datei bestimmen
    fseek(file, 0, SEEK_END);
    long length=ftell(file);
    rewind(file);

    // Speicher für den Shader reservieren
    char * buffer=(void*)malloc(length+1);
    if(buffer==NULL){
        printf("Error allocating memory for shader\n");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, length, file);
    buffer[length]= '\0'; // Nullterminierung	
    fclose(file);
    return buffer;
}

void freeShader(const char *shader){
    free((void*)shader);
}