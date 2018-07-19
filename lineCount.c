int lineCount(FILE *file){
    int count = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF){
        if (ch == '\n'){
            count++;
        }
    }
    return count;
}
/* Funkcia lineCount mi nastavi pointer na koniec suboru.
* preto ho musim vratit na zaciatok suboru.*/
fseek(file, 0, SEEK_SET);
