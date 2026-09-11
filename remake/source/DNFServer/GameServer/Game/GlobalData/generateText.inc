// [b28d-solo-GlobalData] 归零 100=100：P211 档③ 类型旋钮——三个静态计数
// unsigned int→int + 使用处 (unsigned int) cast，复现 `mov <A>,%edx; mov %edx,%ecx`
// 两段 divisor 装载（旧 N18a 判词作废）；CaptchaGen.h 同步（R2 已广播） | ORIG 0x080de83c
void CaptchaGen::generateText(CountryCode code, int len, unsigned short* out)
{
    // [b23S3C] ORIG 0x080de83c：三分支各独立循环（i 槽 -0x14/-0x10/-0xc），
    // 表/计数直引全局静态成员（movzwl table(,%eax,1) 索引形），勿归一成共享循环。
    // [b28 判词·N18a×3] 各分支 `m_rng() % kXxxCount`：ORIG 4.1 先 `mov g,%edx`
    // 再 `mov %edx,%ecx`（divisor 两段物化），4.4 恒直 `mov g,%ecx` 单段（N18a
    // 「4.4 恒折单步 regmove」反向不可造）；三分支共差 3 insn（97 vs 100），
    // div/mod 输入输出逐条等。
    if (code == CountryCode::korea)
    {
        for (int i = 0; i < len; ++i)
        {
            out[i] = kKoreanGeneral[m_rng() % (unsigned int)kKoreanGeneralCount];
        }
    }
    else if (code == CountryCode::japan)
    {
        for (int i = 0; i < len; ++i)
        {
            out[i] =
                kJapaneseHirakana[m_rng() % (unsigned int)kJapaneseHirakanaCount];
        }
    }
    else if (code == CountryCode::taiwan)
    {
        for (int i = 0; i < len; ++i)
        {
            out[i] =
                kTaiwanAlphabet[m_rng() % (unsigned int)kTaiwanAlphabetCount];
        }
    }
}