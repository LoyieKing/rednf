// ORIG T 0x0832ad78：对 20 个 CData* 槽（+0x00..+0x4c）依次调用非虚 CData::reset。
void charac_expand::CDataMgr::reset()
{
    for (int i = 0; i <= 0x13; ++i)
    {
        m_pad[i]->reset();
    }
}