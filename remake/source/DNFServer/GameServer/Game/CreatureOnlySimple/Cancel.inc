void CDBMsgSender::Cancel()
{
    // D1 同形（正向 if 块消 early-return 尾 jmp+nop 中继岛；ORIG 0x833f9a1
    // je 直达 +0x76 leave）。switch 源序 1/0/2 = ORIG 块物理布局
    // +0x27/+0x3f/+0x57（P46 叶块书写序判据）。
    if (m_stream != 0)
    {
        switch (m_stream->m_poolType)
        {
            case 1: GlobalData::s_small_stream_pool->Free(m_stream); break;
            case 0: GlobalData::s_stream_pool->Free(m_stream); break;
            case 2: GlobalData::s_big_stream_pool->Free(m_stream); break;
            default: break;
        }
        m_stream = 0;
    }
