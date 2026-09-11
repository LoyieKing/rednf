// ===========================================================================
// _GetNextAssaultPlaceId —— ORIG T 0x082ed976 (0x87)
// 自增回卷时借静态 count 以 cMyTrace 报警并再 +1。
// ===========================================================================
// [b28d-solo-W6_AssaultMgrDeps] AE（P227 全表达式临时链：ORIG ctor 前 `mov count,%ebx` 跨 ctor
// 保活 0x082ed9a3 逐条复现，39=39；旧 N7/N82 判词作废）；ORIG 0x082ed976。
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
}