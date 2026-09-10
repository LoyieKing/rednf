STGuildDBInfoOnly::STGuildDBInfoOnly()
{
    m_masterNo = 0;                 // +0x17
    m_fundLevel = 0;                // +0x1b
    m_field1E = 0;                  // +0x1e
    m_memberCount = 0;              // +0x22
    m_guildPoint = 0;               // +0x24
    m_guildRank = 0;                // +0x28
    m_fundExp = 0;                  // +0x29
    m_subMasterCnt = 0;             // +0x2d
    m_guildSkillPoint = 0;          // +0x42
    m_skillLearnCnt = 0;            // +0x44
    // ORIG 此处的 16× call STGuildSkill::C1Ev（+0x45 步长 5）由 m_skills 的
    //   隐式成员构造自动生成（CUser.h: STGuildSkill m_skills[16]），无需手写循环。
    m_field95 = 0;                  // +0x95
    m_powerSecedeTime = 0;          // +0x96
    m_powerWarPoint = 0;            // +0x9a
    m_agitExist = 0;                // +0x9e
    m_powerJoinCount = 0;           // +0x9f
    m_fundMoney = 0;                // +0xa0
    m_fieldB9 = 0;                  // +0xb9
    memset(m_subMasterData, 0, 0x14);   // +0x2e
    memset(m_guildName, 0, 0x17);       // +0x00
    // ORIG 0x08693f56：+0x1c 为 byte 位操作（movzbl;or $1;movb / movzbl;and $~2;movb），
    // 只动低字节，不触碰 +0x1d；m_ability 是 unsigned short，需经 byte 视图改写。
    {
        unsigned char* flags = reinterpret_cast<unsigned char*>(&m_ability);
        *flags = (unsigned char)(*flags | 1);
        *flags = (unsigned char)(*flags & ~2);
    }
    memset(m_skills, 0, 0x50);          // +0x45
    memset(m_masterName, 0, 0x15);      // +0xa4
