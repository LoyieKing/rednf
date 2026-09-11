bool CCharacBloodDungeon::loadData(CUser*, char* pData)  // ORIG 0x832a4c6
{
    // pData 即 _saveData 落库的 SIG_BLOOD_DUNGEON 记录（ORIG 0x832a4c6 汇编：
    // 8 条 movl，逐条 sig+0x04/08/0c/10/14/18/20/24 → this+08/0c/10/14/18/1c/24/28）。
    // sig+0x00 characNo 不读取；sig+0x1c（m_flag1c byte）不读取，故 this+0x20
    // m_flag20 保持原值 —— 与 getData 非严格互逆，ORIG 本身即如此。
    const SIG_BLOOD_DUNGEON* sig = reinterpret_cast<const SIG_BLOOD_DUNGEON*>(pData);
    m_point             = sig->m_point;              // sig+0x04（int）
    m_weeklyEnterCount  = sig->m_weeklyEnterCount;   // sig+0x08（int）
    m_useGold           = sig->m_useGold;            // sig+0x0c（int）
    m_field14           = sig->m_dailyInitTime;      // sig+0x10（int）
    m_enterCount        = sig->m_field14;            // sig+0x14（int）
    m_field1c           = sig->m_enterCount;         // sig+0x18（int）
    m_field24           = sig->m_field20;            // sig+0x20（int）
    m_field28           = sig->m_field24;            // sig+0x24（int）
    return true;
}