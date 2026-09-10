void CUser::SetBirth(const char* birth)
{
    // SetBirth.md @0x0867ecf0：strncpy 局部 2 字节缓冲后 atoi；
    // 低字节写 m_birthMonth(+0x8ebe→全址0x711d5)、m_birthDay(全址 0x711d4)。
    // ORIG 0x0867ecf0：入口 `movw $0x0,-0xb(%ebp); movb $0x0,-0x9(%ebp)` 2B+1B 拆分零写
    // （4.1 对 3B 零初始化做 subword store，奇址 -0xb 直落）；memcpy 只覆 [0..1]，尾 0 保持
    // ⇒ 显式 digits[2]='\0' 不存在于 ORIG。4.4 packed struct 复现奇址 HImode store。
    struct __attribute__((packed)) Digits3 {
        short w;
        char b2;
    } digits;
    digits.w = 0;
    digits.b2 = 0;
    // [b28d-solo-CUserOnlyOrig P167 重载选错真 bug] ORIG 0x0867ed18 / 0x0867ed4b
    // 均 call strncpy@plt（0807d8d0），非 memcpy（我方旧写法 EXTRA memcpy ×2、
    // MISSING strncpy ×2）。n=2 且源非零时两者等价，按 ORIG 照抄。
    // [b28d-solo-CUserOnlyOrig P319] ORIG 0x0867ed0a `mov 0xc(%ebp),%eax; add $0x2,%eax`
    // 两段（4.1 指针算术）vs 4.4 `lea 0x2(%eax),%edx` 单段；整型加法+回 cast 复现两段。
    strncpy(reinterpret_cast<char*>(&digits),
            reinterpret_cast<const char*>(reinterpret_cast<unsigned int>(birth) + 2), 2);
    m_birthMonth = static_cast<char>(atoi(reinterpret_cast<char*>(&digits)));
    strncpy(reinterpret_cast<char*>(&digits),
            reinterpret_cast<const char*>(reinterpret_cast<unsigned int>(birth) + 4), 2);
    m_birthDay = static_cast<char>(atoi(reinterpret_cast<char*>(&digits)));
