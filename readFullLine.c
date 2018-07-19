char *readFullLine(FILE *file){
    char *buffer = (char*) malloc(sizeof(char));
    if (!buffer){
        return NULL;
    }
    int ch;
    int count = 0;
    while ((ch = fgetc(file)) != '\n' && ch != EOF){
        buffer[count] = ch;
        count++;
        char *tmp = (char *)realloc(buffer, count*sizeof(ch));
        if (!tmp){
            free(buffer);
            return NULL;
        }
        buffer = tmp;
    }
    buffer[count] = '\0';
    return buffer;
}