void CCreature::Reset()
{
    m_creatureItemType = CREATURE_ITME_TYPE_CREATURE;  // +0x08 = 1（列表包要求 type==1）
    m_uid = 0;                         // +0x0c
    m_slotNo = 0;                      // +0x10
    m_state = 2;                       // +0x3c = 2
    m_saveFlag = 0;                    // +0x40 byte
    m_script = 0;                      // +0x44
    m_creatureId = 0;                  // +0x48
    m_level = 1;                       // +0x2c = 1
    m_exp = 0;                         // +0x30
    m_noCharge = 0;                    // +0x14 byte
    m_isGrow = 1;                      // +0x4c byte
    m_field4d = 5;                     // +0x4d byte
    memset(m_name, 0, 0xd);            // +0x1c memset 13B
    m_stomach.SetStomachValue(0.0f);   // CStomach::SetStomachValue(this+0x34, 0.0)
}
