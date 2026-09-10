bool DNFFLib::ConvertUTF8toGBK(char* src, char* dst) {
    if (CharacSetSwitch("UTF-8", "GBK", src, dst) == false) {
        return 0;
    }
    return 1;
