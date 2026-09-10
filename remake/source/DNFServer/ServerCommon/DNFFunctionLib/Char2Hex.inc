void Char2Hex(unsigned char c, char* out) {
    // ORIG DWARF: saucHex = unsigned char[] (non-const, .data); const 会改变
    // GCC 4.4 -O0 的寄存器分配（edx 直接装载 vs eax->edx 拷贝），须保持非 const。
    static unsigned char saucHex[] = "0123456789abcdef";
    out[0] = saucHex[c >> 4];
    out[1] = saucHex[c & 0xf];
    out[2] = '\0';
