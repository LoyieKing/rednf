// ============================================================================
// QuestParameterScript 奖励段查询（ORIG T 0x08a6bdc6 / 0x08a6be66）。
// getRewardLevel：遍历 m_map3c（map<int,int>），取首个 second<=level 的
// first（初始 0）；getRewardStatus：getRewardLevel(level) 后遍历 m_map54 中
// key<=level 的各 vector，按 Statuepredicate（+0x0/+0x4 两 int）合并：
// 命中段累加 m_value（+0x8 float），否则 push_back（sret 返回）。
// ============================================================================
int QuestParameterScript::getRewardLevel(int level)
{
    int result = 0;
    std::map<int, int>::iterator it = m_map3c.begin();
    // ORIG 0x8a6be2a 调 ppEi（后缀 it++，0x8(%esp)=$0，返回值落废弃临时
    // -0x10），非前缀 ++it。
    for (; it != m_map3c.end(); it++)
    {
        if (it->second <= level)
        {
            result = it->first;
        }
    }
    return result;
}