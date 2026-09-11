// ORIG 0x0817e3e8：6 个桶各自把下标 1..count-1 的 m_nRate（元素 +0x10）累加成前缀和
// push 入 m_cumulativeRate[i]，之前 reserve(count-1)。恒返回 1。
// [b28d-56] 返回类型 int→bool：ORIG Load 0x0817e1bd 对 Init 的调用点形为
// `call; xor $1,%eax; test %al,%al; je`（bool callee 特征，P292/P248 档①）；
// int callee 恒 `test %eax; sete; test`。mangled 不含返回类型，符号/ABI 不变。
bool AvatarFixedHiddenOptionServer::Init()
{
    for (int i = 0; i < 6; ++i)
    {
        int cumulative = 0;
        int count = (int)m_abilities[i].size();
        m_cumulativeRate[i].reserve((std::vector<int>::size_type)(count - 1));
        for (int k = 1; k < count; ++k)
        {
            cumulative += m_abilities[i][k].m_nRate;
            m_cumulativeRate[i].push_back(cumulative);
        }
    }
    return true;
}