int StringToNumber(char const* str) {
    if (str == NULL) {
        return 0;
    }
    return strtoul(str, NULL, 10);
