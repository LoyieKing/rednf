bool DNFFLib::ConvertGBKtoUTF8(char* src, char* dst) {
    // 原始：call CharacSetSwitch 后 xor eax,1; test al,al; je（if (== false) return 0; return 1 形态）
    if (CharacSetSwitch("GBK", "UTF-8", src, dst) == false) {
        return 0;
    }
    return 1;
}
