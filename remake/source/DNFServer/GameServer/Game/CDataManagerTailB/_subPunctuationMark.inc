// ---------------------------------------------------------------------------
// _subPunctuationMark(char const*, char*) —— ORIG T 0x08362c22
//   从 src 中剔除"跳过字符"后拷入 dst：
//     - 单字节：不在 m_slangName.m_charList(+0x86ac) 中 → 复制；
//     - 双字节：word = src[i]|src[i+1]<<8 不在 m_slangName.m_shortList(+0x86b8)
//       中 → 复制两字节。
//   （若命中跳过表则不复制——函数名与两分支对称性一致；见报告汇编逐条核对。）
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManagerTailB] AE：删 strlen 缓存（ORIG 每轮调 strlen）、
//   out 用 int（unsigned 会令 dst[out] 折 lea）、w 用 `unsigned char buf[2]={0}`
//   字节写入形（ORIG movw $0 + 两次 movb + movzwl 回读）。ORIG 0x08362c22。
char* CDataManager::_subPunctuationMark(const char* src, char* dst)
// [P8S6 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
{
    int out = 0;
    for (unsigned int i = 0; i < strlen(src); ++i)
    {
        const unsigned char* p = (const unsigned char*)(src + i);
        if (_isOneByteCharacter(*p))
        {
            if (std::find(m_slangName.m_charList.begin(),
                          m_slangName.m_charList.end(),
                          *p) == m_slangName.m_charList.end())
            {
                dst[out++] = (char)*p;
            }
        }
        else
        {
            // ORIG 取连续两字节组 word（低位在前）
            unsigned char buf[2] = { 0 };
            buf[0] = p[1];
            buf[1] = p[0];
            const unsigned short w = *(const unsigned short*)buf;
            if (std::find(m_slangName.m_shortList.begin(),
                          m_slangName.m_shortList.end(),
                          w) == m_slangName.m_shortList.end())
            {
                dst[out++] = (char)*p;
                dst[out++] = (char)p[1];
            }
            // [l5f 2026-09-11 · 真语义修复] ORIG 0x08362ddf 的 addl $1,-0x10(%ebp)
            // 在 find 判定**之外**（`je 08362ddf` 与 fall-through 同落该点）⇒
            // 双字节字符无论是否命中跳过表都消耗 2 字节；旧码把 ++i 写在 if 体内
            // ⇒ 未命中时只前进 1 字节（第二字节被当新字符处理），行为分叉。
            ++i;  // 双字节多消耗一个输入位（asm +0x1bd 额外自增，与循环尾 +0x1c1 合计 2）
        }
    }
    dst[out] = '\0';
    return dst;
}