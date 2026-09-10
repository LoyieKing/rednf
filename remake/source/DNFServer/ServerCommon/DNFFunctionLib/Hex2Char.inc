bool Hex2Char(char const* hex, unsigned char& out) {
    // 原始：数字分支在前（cmp 0x2f/0x39），hex 分支其次（cmp 0x60/0x66），失败最后
    if (hex[0] > '/' && hex[0] <= '9') {
        out = hex[0] - '0';
    } else if (hex[0] > '`' && hex[0] <= 'f') {
        out = hex[0] - 'a' + 10;
    } else {
        return 0;
    }
    // 原始：指针递增 hex++（add DWORD PTR [ebp+0x8],0x1）
    hex++;
    if (hex[0] > '/' && hex[0] <= '9') {
        // 原始：两步赋值（out<<4 先存回 out，再与低半字节相加）
        out = (unsigned char)(out << 4);
        out = (unsigned char)(out + (hex[0] - '0'));
    } else if (hex[0] > '`' && hex[0] <= 'f') {
        out = (unsigned char)(out << 4);
        out = (unsigned char)(out + (hex[0] - 'a' + 10));
    } else {
        return 0;
    }
    return 1;
