void DNFFLib::PrintTextFile(char* file, char* text) {
    FILE* f = fopen(file, "a");
    if (f != NULL) {
        fprintf(f, text);  // 原始：文本直接作为格式串
        fclose(f);
    }
}
