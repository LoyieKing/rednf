unsigned int CAssaultMgr::_GetNextAssaultPlaceId()
{
    static int count = 0;                       // ORIG _ZZN…_GetNextAssaultPlaceIdE5count

    ++m_nextAssaultPlaceId;
    if (m_nextAssaultPlaceId == 0)
    {
        ++count;
        // [b28d-solo] P227 全表达式临时链：ORIG 在 ctor 前 `mov count,%ebx`
        // 跨 ctor call 保活（0x082ed9a3），具名 trace 形 4.4 恒重读 count。
        cMyTrace(__PRETTY_FUNCTION__, 0x1085, 0)(
            "NEXT_ASSAULT_PLACE_ID overflow count(%d)", count);
        ++m_nextAssaultPlaceId;
    }
    return m_nextAssaultPlaceId;
