void DNFFLib::Binary2Hex(unsigned char const* data, int len, char* out) {
    // ORIG DWARF：局部 i(67)/szHex(68)/pucBinStr1(69)，声明不初始化，
    // 体中按 pucBinStr1→out[0]→i 序赋值（同 wrapper 版形态）。
    int i;
    char szHex[3];
    unsigned char const* pucBinStr1;
    pucBinStr1 = data;
    out[0] = '\0';
    i = 0;
    while (i < len) {
        Char2Hex(*pucBinStr1, szHex);
        strcat(out, szHex);
        // ORIG：i 先自增、指针后自增。
        i++;
        pucBinStr1++;
    }
}
