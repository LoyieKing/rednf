bool DNFFLib::Hex2Binary(char const* hex, unsigned char* out, int maxLen) {
    int count = 0;
    while (count < maxLen) {
        unsigned char value;
        if (Hex2Char(hex, value) != 1) {
            // 原始：Hex2Char 失败即返回 0
            return 0;
        }
        // 原始：*out = value 后按 count→hex→out 序自增。
        *out = value;
        count++;
        hex += 2;
        out++;
    }
    return 1;
