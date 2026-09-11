// ORIG _ZN9CRijndael10ResetChainEv T @0x080b6a54：
//   !m_bInit → 0x70000005；否则 chain0(+0x3dc) → chain(+0x3fc) 按块长复制，
//   成功返 0x6fffffff。
int CRijndael::ResetChain()
{
    if (!m_bInit)
    {
        return 0x70000005;
    }
    std::memcpy(m_chain, m_block, m_blockSize);
    return 0x6fffffff;
}