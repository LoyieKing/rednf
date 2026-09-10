int decryptBVHackPacket(char* payload, stBVHackInfo_t& info)
{
    if (payload == 0)                               // 0x088920f7
        return 0;

    // [b28d-lane15] ORIG 0x08892107 `movl $0x6e3f4b9c,-0x10(%ebp)` 是**死写入**局部
    // （比较仍用立即数 `cmp $0x6e3f4b9c`）——P171 照抄，勿删。
    unsigned int magic = 0x6e3f4b9cu;

    if (*reinterpret_cast<unsigned int*>(payload) != 0x6e3f4b9cu)   // 0x08892113
        return 0;

    payload += 4;                                   // addl $0x4,0x8(%ebp)

    const unsigned int key =
        *reinterpret_cast<unsigned int*>(payload + 0x30);           // mov 0x30(%eax)

    unsigned int* cur = reinterpret_cast<unsigned int*>(payload);   // -0x8 游标
    // ORIG 0x0889215b `setbe`（无符号 ≤）⇒ 循环变量必须 unsigned（P2）。
    for (unsigned int i = 0; i <= 11; ++i)          // cmpl $0xb … setbe（i≤11 共 12 次）
    {
        *cur = *cur ^ key;                          // mov (%edx); xor -0xc; mov (%eax)
        ++cur;
    }

    // 6×double：ORIG 逐次 `fldl (payload); fstpl off(info); addl $0x8,payload`
    // （形参自增，非数组下标 src[i]）。
    info.m_values[0] = *reinterpret_cast<double*>(payload); payload += 8;
    info.m_values[1] = *reinterpret_cast<double*>(payload); payload += 8;
    info.m_values[2] = *reinterpret_cast<double*>(payload); payload += 8;
    info.m_values[3] = *reinterpret_cast<double*>(payload); payload += 8;
    info.m_values[4] = *reinterpret_cast<double*>(payload); payload += 8;
    info.m_values[5] = *reinterpret_cast<double*>(payload); payload += 8;
    return 1;                                       // mov $0x1,%eax
