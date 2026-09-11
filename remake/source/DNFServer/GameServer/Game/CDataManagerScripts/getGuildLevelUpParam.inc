// ---- GuildParameterScript::getGuildLevelUpParam（ORIG T 0x08979648）----
// level∈[1,16] 返回 m_levelUpCost+(level-1)*8 的地址（按 int 口径），否则 0。
int GuildParameterScript::getGuildLevelUpParam(int level)
{
    // [b28c-L29] ORIG 0x08979648：负极性短路 `level<=0||level>0x10` 的 4.4 复现
    // 形=正极性包裹（P214①）：&& 两失败项各自 jle/jg 跳同一 return 0 尾块；
    // 返回 m_levelUpCost+(level-1) 指针算术形生 lea 0x5c(%eax),%edx 链
    // （探针 l29_probe.cpp f5 逐条复现）。
    if (level > 0 && level <= 0x10)
        return reinterpret_cast<int>(m_levelUpCost + level - 1);
    return 0;
}