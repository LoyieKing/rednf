CStreamGuard::~CStreamGuard()
{
    if (m_stream && m_bOwn && m_stream->m_inUse)
    {
        // ORIG cmp 级联顺序：1→Small、2→Big、default→Stream
        switch (m_stream->m_poolType)
        {
        case 1:
            GlobalData::s_small_stream_pool->Free(m_stream);
            break;
        case 0:
            GlobalData::s_stream_pool->Free(m_stream);
            break;
        case 2:
            GlobalData::s_big_stream_pool->Free(m_stream);
            break;
        }
    }
}