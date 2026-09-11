// [b28d-solo-STMapScript] _Z19convertBinaryToTextPKc：AE（离开 DIFF）。
// 修法：声明序 fp 先于 count（ORIG -0x14/-0x10）、for(;;)+两个 break（复现 ORIG 0x088be085/
// 0x088be092 双 nop 岛）、fwrite 实参序 (buf,count,1,fp)。p167 CLEAR 8/8。
void convertBinaryToText(const char* path)
{
    static char buffer[0x5b400];   // ORIG _ZZ19convertBinaryToTextPKcE6buffer 0x0949b4e0
    FILE* fp = fopen(toMbcs(path), "rb");                 // -0x14(%ebp)（ORIG 声明序 fp 先）
    int count = 0;                                        // -0x10(%ebp)
    for (;;)                                              // ORIG 0x088be04c：循环头在体内
    {
        int c = fgetc(fp);                                // -0xc(%ebp)
        if (c == -1)                                      // 0x088be085 岛
            break;
        if (count > 0x5b3ff)                              // 缓冲上限（0x088be092 岛）
            break;
        if (c == '\n')
        {
            // 首字符或前一字符非 '\r' 时补 '\r'（\n 归一为 \r\n）
            if (count == 0 || buffer[count - 1] != '\r')
                buffer[count++] = '\r';
        }
        buffer[count++] = static_cast<char>(c);
    }
    fclose(fp);
    fp = fopen(toMbcs(path), "wb");                       // 0x08e086af
    fwrite(buffer, count, 1, fp);                         // ORIG 实参序 (buf,count,1,fp)
    fclose(fp);
}