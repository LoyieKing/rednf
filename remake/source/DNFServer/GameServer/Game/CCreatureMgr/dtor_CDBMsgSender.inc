CDBMsgSender::~CDBMsgSender()
{
    // ORIG 0x833f8e5：je 直达尾 leave（正向 if 块，body fall-through 收尾，
    // 避免 4.4 early-return 在 leave 前产 jmp+nop 岛）。
    if (m_stream != 0)
    {
        // ORIG 0x833f8ef 下探树 cmp1/cmp2/test0；块布局 1@0x8ff、0@0x917、2@0x92f；
        // default 无动作（0x8fd jmp 尾）。case0 必须显式（否则 default 吞 0 消 test）。
        switch (m_stream->m_poolType)
        {
            case 1: GlobalData::s_small_stream_pool->Free(m_stream); break;
            case 0: GlobalData::s_stream_pool->Free(m_stream); break;
            case 2: GlobalData::s_big_stream_pool->Free(m_stream); break;
            default: break;
        }
        m_stream = 0;
    }
