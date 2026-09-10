int DoSuperFastHash(char* data, unsigned int len, unsigned int& hash)
{
    hash = 0;
    unsigned int rem = len;
    if (len == 0 || data == 0)
        return 0;

    // [SweepPIHash] ORIG t 槽 -0x8 深于 tmp 槽 -0x4 ⇒ t 须声明在 tmp 之前
    // （4.4 函数头标量按声明序「先声明深」，与 ORIG 一致）。
    unsigned int t;
    unsigned int tmp = len & 3;
    for (len >>= 2; len != 0; --len)
    {
        rem += ((unsigned int)(unsigned char)data[1] << 8) +
               (unsigned int)(unsigned char)data[0];
        // ORIG 用 `(data[3]<<8) + data[2]`（lea 合并），非 `|`（or 形）。
        t = (((unsigned int)(unsigned char)data[3] << 8) +
             (unsigned int)(unsigned char)data[2]) << 11 ^ rem;
        rem = (rem << 16) ^ t;
        data += 4;
        rem += rem >> 11;
    }
    if (tmp == 2)
    {
        rem += ((unsigned int)(unsigned char)data[1] << 8) +
               (unsigned int)(unsigned char)data[0];
        rem ^= rem << 11;
        rem += rem >> 17;
    }
    else if (tmp == 3)
    {
        rem += ((unsigned int)(unsigned char)data[1] << 8) +
               (unsigned int)(unsigned char)data[0];
        rem ^= rem << 16;
        rem ^= (unsigned int)(int)(signed char)data[2] << 18;
        rem += rem >> 11;
    }
    else if (tmp == 1)
    {
        // ORIG movsx（signed char）——与 tmp==3 的 data[2] 一致。
        rem += (unsigned int)(int)(signed char)data[0];
        rem ^= rem << 10;
        rem += rem >> 1;
    }
    rem ^= rem << 3;
    rem += rem >> 5;
    rem ^= rem << 4;
    rem += rem >> 17;
    rem ^= rem << 25;
    // ORIG 分两步（`add %eax,-0xc(%ebp)` 就地更新 rem 再读入 hash），
    // 单表达式 `hash = rem + (rem>>6)` 4.4 恒产 edx 中转形。
    rem += rem >> 6;
    hash = rem;
    return 1;
