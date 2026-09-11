// ORIG 0x833f280：if (m_field4c != 0) return 0;
//   if (m_field5c==itemIdx || m_field60==itemIdx) return 1 else 0。
bool CCreatureScript::validItemExp(int itemIdx)
{
    // [b28d-34] P0 清绑（N96+N18a 封顶）：原 `register unsigned char v asm("eax")`
    // 违反 P0 全域禁则。ORIG 0x833f280 `movzbl 0x4c;xor$1;test;je` 寄存器物化
    // 无绑定不可达（P168a④），以 `cmpb $1,mem` 最短形替代。
    if (m_field4c != 1)
        return false;
    if (m_field5c == itemIdx || m_field60 == itemIdx)
    {
        return true;
    }
    return false;
}