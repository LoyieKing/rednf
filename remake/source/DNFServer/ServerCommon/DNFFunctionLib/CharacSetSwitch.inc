bool DNFFLib::CharacSetSwitch(char const* from, char const* to, char* src, char* dst) {
    char* SrcPtr = NULL;
    char* DstPtr = NULL;
    size_t size1 = 0;
    size_t string_size = 0;
    size_t size2 = 0;
    size_t cc;
    iconv_t it = iconv_open(to, from);
    if (it == (iconv_t)-1) {
        printf("iconv_open error : %s\n", strerror(errno));
        return 0;
    }
    SrcPtr = src;
    DstPtr = dst;
    size1 = strlen(SrcPtr);
    string_size = size1 * 3;
    size2 = string_size;
    cc = iconv(it, &SrcPtr, &size1, &DstPtr, &string_size);
    if (cc == (size_t)-1) {
        printf("iconv error : %s\n", strerror(errno));
        return 0;
    }
    dst[size2 - string_size] = '\0';
    iconv_close(it);
    return 1;
}
