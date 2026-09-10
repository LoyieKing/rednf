void DNFFLib::Make_Dir(char* path) {
    DIR* dir = opendir(path);
    if (dir == NULL) {
        mkdir(path, 0x1ff);
    }
