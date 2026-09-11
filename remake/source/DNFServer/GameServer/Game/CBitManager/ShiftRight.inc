// [N5A 2026-09-11] 旧「变长移位计数掩码 N 类」判词**已推翻**：3 处 `and $0x1f`
// 全部来自我方显式 `bits & 0x1f`（4.4 对 `%cl` 变长移位**不**插掩码）。
// 改写为 ORIG 形「int i=0; for(i=m_size-1;i>=0;--i) + buf[i] | (buf[i-1]<<(8-bits))」
// 后探针（/tmp/r5/n5/N5A_BattleField/sr_probe.cpp 变体 A）逐条全等 ORIG 0x0831c1d6
// （含 push %edi、槽 -0x10、not/shr 尾测）⇒ ORIG 57 / 我方 57。
void CBitManager::ShiftRight(int bits, unsigned char* buf)  // 0x0831c1d6
{
    int i = 0;
    for (i = m_size - 1; i >= 0; --i)
    {
        buf[i] = static_cast<unsigned char>(buf[i] >> bits);
        if (i != 0)
        {
            buf[i] = static_cast<unsigned char>(buf[i] | (buf[i - 1] << (8 - bits)));
        }
    }
}