// ============================================================================
// CVillageMonster 构造/析构/GetUID —— ORIG 0x86b3004 / 0x86b3098 / 0x86b3106 /
// 0x86b4b64。7 参 ctor：m_area=area；m_id=uid_seed 自增分配（清零则回 1）；
// m_zone=id；m_field04=x；m_field08=y；m_field10=0；m_field0c=slot；
// m_partyIndex=-1；m_state=0；m_field20/m_field24 直接落位。
// ============================================================================
CVillageMonster::CVillageMonster()
{
    m_area = 0;
    m_id = 0;
    m_zone = 0;
    m_field04 = 0;
    m_field08 = 0;
    m_field10 = 0;
    m_field0c = 0;
    m_partyIndex = -1;
    m_state = ENUM_VILLAGE_MONSTER_STATE_0;   // b28d：m_state 已 enum 化（CVillageMonsterMgr.h）
    m_field20 = 0;
    m_field24 = 0;
}