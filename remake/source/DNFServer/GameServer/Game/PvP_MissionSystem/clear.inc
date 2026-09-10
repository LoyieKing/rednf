void MissionScript::clear()
{
    // [b28c-lane17] ORIG 0x08a5bbc6：pair 临时构造（两 int 0 槽）+ operator=。
    m_grade = std::pair<int, int>(0, 0);   // +0x10/+0x14
    m_field20 = 4;               // +0x20
    m_job = 0xb;                 // +0x24（无职业）
    m_field28.clear();
    m_field34 = 0x10;            // +0x34（默认 kind）
    m_field00 = -1;              // +0x00（[next mission]）
    m_prevMissionIndex = -1;     // +0x04（[prev mission]）
    m_str38.clear();
    m_str3c.clear();
    m_rewardItems.clear();
    m_expRate = 0.0f;            // +0x40
    m_stealingSkill = 0;         // +0x5c
    m_rewardSkill = 0;           // +0x5d
    m_rewardSp = 0;              // +0x5e
    m_comboMap.clear();
    m_field08 = 0;
    m_field0c = 0;
