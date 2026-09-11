// ORIG W 0x0844d576：mov 0x8(%ebp),%eax; add $0x140,%eax; ret
// —— 返回 this+0x140（m_rankRate 对象地址），即 vector 引用。
std::vector<int>& RefPvpGrade::getPvPRankRate()
{
    return m_rankRate;
}