village_attacked::CVillageMonster::CVillageMonster(
    unsigned short id, int x, int y, int slot,
    village_attacked::CVillageMonsterArea* area, int field20, int field24)
{
    static int uid_seed = 0;   // ORIG d 0x093ace54（函数内静态）
    m_area = area;
    m_id = (unsigned short)uid_seed++;
    if (uid_seed == 0)
        uid_seed = 1;
    m_zone = id;
    m_field04 = x;
    m_field08 = y;
    m_field10 = 0;
    m_field0c = slot;
    m_partyIndex = -1;
    m_state = ENUM_VILLAGE_MONSTER_STATE_0;   // b28d：m_state 已 enum 化（CVillageMonsterMgr.h）
    m_field20 = field20;
    m_field24 = field24;
