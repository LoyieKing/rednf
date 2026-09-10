void CCharacBloodDungeon::getData(char* pData) const  // ORIG 0x832a53a
{
    // loadData 的反向写出（ORIG 0x832a53a：8 条 movl + 1 条 movzbl/movb）。
    SIG_BLOOD_DUNGEON* sig = reinterpret_cast<SIG_BLOOD_DUNGEON*>(pData);
    sig->m_point             = m_point;                      // this+0x08 → sig+0x04
    sig->m_weeklyEnterCount  = m_weeklyEnterCount;           // this+0x0c → sig+0x08
    sig->m_useGold           = m_useGold;                    // this+0x10 → sig+0x0c
    sig->m_dailyInitTime     = m_field14;                    // this+0x14 → sig+0x10
    sig->m_field14           = m_enterCount;                 // this+0x18 → sig+0x14
    sig->m_enterCount        = m_field1c;                    // this+0x1c → sig+0x18
    sig->m_flag1c            = m_flag20;                     // this+0x20 → sig+0x1c（byte）
    sig->m_field20           = m_field24;                    // this+0x24 → sig+0x20
    sig->m_field24           = m_field28;                    // this+0x28 → sig+0x24
