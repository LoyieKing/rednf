// b28d-GrpD 按 ORIG 0x08693f56 直证改写（两处）：
//  ① ORIG 生成序 = [0x17..0x44 逐字段 movl/movb/movw] → [16× call STGuildSkill::C1Ev
//     循环 @+0x62] → [0x95..0xb9 逐字段 @+0x7f] → [函数体 memset…]。循环夹在标量
//     赋值**中间**，只可能是**成员初始化列表**（gcc 按声明序：m_skillLearnCnt@0x44
//     → m_skills@0x45 隐式构造 → m_field95@0x95）；我方旧「体内赋值」写法会把
//     m_skills 隐式构造顶到函数最前（实测我方 -0x20 帧、循环在首条）。
//     逐条指令核对：+0x0b movl 0x17 / +0x11 movb 0x1b / +0x18 movl 0x1e / +0x1f movw 0x22
//     / +0x26 movl 0x24 / +0x2d movb 0x28 / +0x34 movl 0x29 / +0x3b movb 0x2d /
//     +0x42 movw 0x42 / +0x49 movb 0x44 —— 与下列列表顺序一一对应。
STGuildDBInfoOnly::STGuildDBInfoOnly()
    : m_masterNo(0)                 // +0x17
    , m_fundLevel(0)                // +0x1b
    , m_field1E(0)                  // +0x1e
    , m_memberCount(0)              // +0x22
    , m_guildPoint(0)               // +0x24
    , m_guildRank(0)                // +0x28
    , m_fundExp(0)                  // +0x29
    , m_subMasterCnt(0)             // +0x2d
    , m_guildSkillPoint(0)          // +0x42
    , m_skillLearnCnt(0)            // +0x44
    , m_field95(0)                  // +0x95
    , m_powerSecedeTime(0)          // +0x96
    , m_powerWarPoint(0)            // +0x9a
    , m_agitExist(0)                // +0x9e
    , m_powerJoinCount(0)           // +0x9f
    , m_fundMoney(0)                // +0xa0
    , m_fieldB9(0)                  // +0xb9
{
    memset(m_subMasterData, 0, 0x14);   // +0x2e
    memset(m_guildName, 0, 0x17);       // +0x00
    // ② ORIG +0x47：+0x1c 为 byte 位操作（`mov 0x8(%ebp),%eax; movzbl 0x1c(%eax),%edx;
    //    or $0x1,%edx; mov %dl,0x1c(%eax)` / `…and $0xfffffffd…`），只动低字节、不触碰
    //    +0x1d；m_ability 是 unsigned short ⇒ 必须经 byte 左值直接改写（P183：ORIG
    //    无 `unsigned char* flags` 具名指针槽，我方旧形多 -0xc 槽、帧 0x20 vs ORIG 0x10）。
    *(unsigned char*)&m_ability = (unsigned char)(*(unsigned char*)&m_ability | 1);
    *(unsigned char*)&m_ability = (unsigned char)(*(unsigned char*)&m_ability & ~2);
    memset(m_skills, 0, 0x50);          // +0x45
    memset(m_masterName, 0, 0x15);      // +0xa4
