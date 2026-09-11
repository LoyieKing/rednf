// ---- GuildParameterScript::getLevelupPrice（ORIG T 0x089797a4）----
// 自高向低扫 m_vecF0，首个 first<=memberCnt（members 阈值）的
// second[level]；全不命中取 [0].second[level]；结果 ×10000。
// [b28d-solo-CDataManagerScripts] AE：删 tbl 引用缓存与 priceVecIdx（P183 每点全表达式 m_vecF0[i] + 命中直接 return）⇒ 循环变量 -0xc 复用、无 -0x14/-0x10 缓存槽；ORIG 0x089797a4。
int GuildParameterScript::getLevelupPrice(int level, int memberCnt)
{
    for (int i = (int)m_vecF0.size() - 1; i >= 0; --i)
    {
        if (m_vecF0[i].first <= memberCnt)
            return m_vecF0[i].second[level] * 10000;
    }
    return m_vecF0[0].second[level] * 10000;
}