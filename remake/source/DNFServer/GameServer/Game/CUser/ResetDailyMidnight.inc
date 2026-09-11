// ORIG 0x0832ade8：对 20 个成员（+0x00..+0x4c，各为 charac_expand::CData*）
// 依次调用虚表槽 0（ResetDailyMidnight）。
// ORIG 0x0832adaa：同上，调用虚表槽 3（ResetDaily）。
void charac_expand::CDataMgr::ResetDailyMidnight()
{
    for (int i = 0; i < 0x14; ++i)
    {
        m_pad[i]->ResetDailyMidnight();
    }
}