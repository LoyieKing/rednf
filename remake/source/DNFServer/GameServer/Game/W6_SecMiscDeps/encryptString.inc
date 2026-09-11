// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N4/P75）→ 试=新口径 n1_v 100/101 gap=-1；前任三形（`*(length+out)` 整型在左不变；整型加法+回 cast 编译非法；删 (size_t) cast 不变）+ 本波逐条复核不变 ⇒ 维持（ORIG 0x085fe417 `add 0x10(%ebp),%eax` 值上下文 vs 我方 lea 2 条）。
char* WongWork::CSecurityCardCenter::encryptString(char const* src, char* out)
{
    // [b28d-solo-W6_SecMiscDeps 判词 · N 封顶（P75 实参装载序 + P30）；105→19]
    // ORIG 0x085fe2e2：帧 0x220。
    // [M2FreeNext15A · 2026-09-11 复核] gap=−1（ORIG 100 / our 101）：多的一条是
    // Encrypt 段 `mov -0x10(%ebp),%ecx` 中转（ORIG 复用 %edx 直压 0xc(%esp)）；
    // 本波试过「删 (size_t)paddedLength 冗余 cast」（P41 变体）⇒ our 101 不变、
    // TU 四数 62/140/0/4 逐值不变 ⇒ 维持 P75 装载时刻封顶。
    // 其余差异：Pad/Binary2Hex 实参装载序（ORIG 先物化本地数组 lea 保活）、
    // `out[length]` 结尾 add-vs-lea（N4）——均为 N 族。
    // **真语义 bug 修正**：旧 `memset(plain,0,0x40)` 少清 0xc0 字节——ORIG 两处
    // `mov $0x40,%edx; …; rep stos` 的 0x40 是 **dword 计数**（0x40×4=0x100 字节，
    // P116/P253）⇒ 必须 `char plain[0x100] = {0}`（P214③ 复现 rep stos）。
    // 另：paddedLength 预清零（P198①）、length 槽复用为 hexLength、异常路径直接
    // `return 0`（ORIG ebx=0，无 result 局部）。
    // ORIG T 0x085fe2e2 (size 0x161)。两块 256 字节栈缓冲清零 → strncpy 0x100
    // → strlen → IMethod::GetBlockSize 取块大小并向上取整 → Pad → CRijndael::
    // Encrypt（asm 为非虚派发直呼）→ Binary2Hex 转十六进制并以 len*2+1 下标补
    // '\0'。异常路径返回 NULL（ORIG catch 吞掉后 ebx=0）。
    // [N1A 2026-09-11 负结论 · 封顶] 本波三试不可造 ORIG 形：
    // ①`*(length + out) = '\0'`（整型在左）实测逐条不变（4.4 归一）；
    // ②P319 整型加法+回 cast 形编译非法（static_cast<char*>→unsigned int），已回滚；
    // ③（历史）删冗余 cast 亦不变。残余四处 = Pad/Encrypt/Binary2Hex 实参装载序
    //   （同条数异序）+ `out[length]` 我方 lea(2 条) vs ORIG `add 0x10(%ebp),%eax`
    //   (1 条，值上下文，0x085fe417) ⇒ 4.4 地址上下文固有选择 ⇒ gap −1 封顶。
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
}