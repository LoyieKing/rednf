char* WongWork::CSecurityCardCenter::encryptString(char const* src, char* out)
{
    // [b28d-solo-W6_SecMiscDeps 判词 · N 封顶（P75 实参装载序 + P30）；105→19]
    // ORIG 0x085fe2e2：帧 0x220。
    // **真语义 bug 修正**：旧 `memset(plain,0,0x40)` 少清 0xc0 字节——ORIG 两处
    // `mov $0x40,%edx; …; rep stos` 的 0x40 是 **dword 计数**（0x40×4=0x100 字节，
    // P116/P253）⇒ 必须 `char plain[0x100] = {0}`（P214③ 复现 rep stos）。
    // 另：paddedLength 预清零（P198①）、length 槽复用为 hexLength、异常路径直接
    // `return 0`（ORIG ebx=0，无 result 局部）。
    // ORIG T 0x085fe2e2 (size 0x161)。两块 256 字节栈缓冲清零 → strncpy 0x100
    // → strlen → IMethod::GetBlockSize 取块大小并向上取整 → Pad → CRijndael::
    // Encrypt（asm 为非虚派发直呼）→ Binary2Hex 转十六进制并以 len*2+1 下标补
    // '\0'。异常路径返回 NULL（ORIG catch 吞掉后 ebx=0）。
    char plain[0x100] = {0};
    unsigned char cipher[0x100] = {0};
    strncpy(plain, src, 0x100);
    size_t length = strlen(plain);
    int paddedLength = 0;
    try
    {
        const int blockSize = m_cipher.GetBlockSize((unsigned int*)0);
        // 块序照 ORIG：`js L_neg` 远跳 + else 块落函数尾（内部 rem==0 的 je 落中部）
        if (blockSize >= 0)
        {
            if ((int)length % blockSize != 0)
            {
                paddedLength = ((int)length / blockSize + 1) * blockSize;
            }
            else
            {
                paddedLength = (int)length;
            }
        }
        else
        {
            paddedLength = (int)length;
        }
        if (blockSize >= 0)
        {
            m_cipher.Pad(plain, (int)length, (unsigned int*)0);
        }
        m_cipher.Encrypt(plain, reinterpret_cast<char*>(cipher),
                         (size_t)paddedLength);
        length = (size_t)(paddedLength * 2 + 1);
        DNFFLib::Binary2Hex(cipher, paddedLength, out);
        out[length] = '\0';
    }
    catch (...)
    {
        return 0;   // ORIG __cxa_begin_catch/__cxa_end_catch → ebx=0
    }
    return out;
