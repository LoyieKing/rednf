// ORIG _ZN4CTEA10ResetChainEv T @0x080c35a8：
//   !m_bInit → 0x70000005；否则 chain(+0x38) 复制回 chain2(+0x40)，
//   长度取 m_blockSize(this+0x08)，成功返 0x6fffffff。
int CTEA::ResetChain()
{
    if (!m_bInit)
    {
        return 0x70000005;
    }
    std::memcpy(m_chain2, m_chain, m_blockSize);
    return 0x6fffffff;
}