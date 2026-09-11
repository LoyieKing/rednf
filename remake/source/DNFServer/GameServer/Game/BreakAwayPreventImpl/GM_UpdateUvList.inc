int CBreakAwayPreventSystem::GM_UpdateUvList(unsigned int accId,
                                             unsigned int day)
{
    memset(m_tail, 0, 0x1f);   // ORIG memset(this+0x30b,0,0x1f)
    // ORIG 决策树（0x0831ca80..0x0831cb27）：day 1/2/3 各独立 memcpy 同串
    // （0x8c242c0），day 4 用另一串（0x8c242e0），default 无 memcpy。
    switch (day)                        // ORIG switch ebp+0x10
    {
    case 1:
        memcpy(m_tail, kUvListDay123, 0x1f);   // ORIG 块 @0x4c 0x8c242c0
        break;
    case 2:
        memcpy(m_tail, kUvListDay123, 0x1f);   // ORIG 块 @0x6e 0x8c242c0
        break;
    case 3:
        memcpy(m_tail, kUvListDay123, 0x1f);   // ORIG 块 @0x90 0x8c242c0
        break;
    case 4:
        memcpy(m_tail, kUvListDay4, 0x1f);     // ORIG 块 @0xb2 0x8c242e0
        break;
    default:
        break;   // ORIG default 无 memcpy
    }
    DB_UpdateBreakAwayUvList::makeRequest(
        accId, m_tail);   // ORIG 0x0831cb3e
    CLocalChina_DB_BreakAwayReset::makeRequest(accId);   // ORIG static (no this) 0x0831cb49
    return 1;   // ORIG 0x0831cb4e mov $0x1,%eax
}