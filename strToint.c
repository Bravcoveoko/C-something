int strToint(char *str){
    /* "Pretypovanie"
     * Vyuzivam ASCII hodnotu cisla.*/
    int result = 0;
    for (size_t i = 0; i < strlen(str); i++){
        result = result * 10 + (str[i] - '0');
    }
    return result;
}