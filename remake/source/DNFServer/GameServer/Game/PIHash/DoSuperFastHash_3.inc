int DoSuperFastHash(unsigned char* data, unsigned int len, unsigned int& hash)
{
    // [b28d-L1] 同 char* 版修法（ORIG W 0x0808c174）：len 作循环变量、单一 rem
    // 累加、分步就地 `rem op=`、逐字节 movzbl 组装。
    // [SweepPIHash] 续修同 char* 版（139/138）：`(data[3]<<8)+data[2]`、t 前置声明、
    // 尾部 `rem += rem>>6; hash = rem;` 分两步；残余为尾部 if 链内存直比（N）。
    hash = 0;
    unsigned int rem = len;
    if (len == 0 || data == 0)
        return 0;

    // [SweepPIHash] 同 char* 版：t 声明前置于 tmp（ORIG t@-0x8 / tmp@-0x4）。
    unsigned int t;
    unsigned int tmp = len & 3;
    for (len >>= 2; len != 0; --len)
    {
        rem += ((unsigned int)data[1] << 8) + (unsigned int)data[0];
        // ORIG `(data[3]<<8) + data[2]`（lea），非 `|`。
        t = (((unsigned int)data[3] << 8) + (unsigned int)data[2]) << 11 ^ rem;
        rem = (rem << 16) ^ t;
        data += 4;
        rem += rem >> 11;
    }
    if (tmp == 2)
    {
        rem += ((unsigned int)data[1] << 8) + (unsigned int)data[0];
        rem ^= rem << 11;
        rem += rem >> 17;
    }
    else if (tmp == 3)
    {
        rem += ((unsigned int)data[1] << 8) + (unsigned int)data[0];
        rem ^= rem << 16;
        rem ^= (unsigned int)(int)(signed char)data[2] << 18;
        rem += rem >> 11;
    }
    else if (tmp == 1)
    {
        rem += (unsigned int)(int)(signed char)data[0];
        rem ^= rem << 10;
        rem += rem >> 1;
    }
    rem ^= rem << 3;
    rem += rem >> 5;
    rem ^= rem << 4;
    rem += rem >> 17;
    rem ^= rem << 25;
    // 同 char* 版：ORIG 分两步就地更新 rem 再读入 hash。
    rem += rem >> 6;
    hash = rem;
    return 1;
