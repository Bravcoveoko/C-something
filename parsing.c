void parsing_and_encoding(char *arr, char pattern){
    // Pole pointrov
    char **buffer = (char**) malloc(sizeof(char*));
    // dlzka zadaneho riadku
    size_t length = strlen(arr);
    int count = 0;
    buffer[count] = arr;
    for (size_t i = 0; i < length; i++){
        if (arr[i] == pattern){
            count++;
            // Na indexe patternu dam '\0' a tym sa mi cely riadok rozdeli na sub-stringy.
            arr[i] = '\0';
            char **pOld = buffer;
            // Naalokujem si dalsie miesto pre dalsi pointer.
            buffer = (char **) realloc(buffer, (count+1)*sizeof(char*));
            if (!buffer){
                free(pOld);
                return;
            }
            // Priradim pointer
            buffer[count] = (arr + i + 1);
        }
    }
    /*vypis:
    for (int i = 0; i <= count; i++){
        printf("%s\n", *(buffer + i));
    }*/
}