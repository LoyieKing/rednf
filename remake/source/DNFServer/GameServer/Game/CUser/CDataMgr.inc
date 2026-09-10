charac_expand::CDataMgr::CDataMgr()
{
    // slot0 +0x00：ORIG operator new(0x30) → memset 全块 → CCharacExpertJob::CCharacExpertJob()
    void* mem0 = ::operator new(0x30);
    memset(mem0, 0, 0x30);
    expert_job::CCharacExpertJob* expert =
        new (mem0) expert_job::CCharacExpertJob();   // placement ctor（ORIG memset 后 ctor）
    m_pad[0] = expert;

    // slot1 +0x04：operator new(0x2c) → memset → CCharacBloodDungeon
    void* mem1 = ::operator new(0x2c);
    memset(mem1, 0, 0x2c);
    CCharacBloodDungeon* blood = new (mem1) CCharacBloodDungeon();
    m_pad[1] = blood;

    // slot2..slot19：ORIG 纯 new + 默认 ctor（无 memset）
    m_pad[2] = new item_lock::CItemLock();            // new(0x58)
    m_pad[3] = new CPvPLiveEventData();                  // new(0xc)
    m_pad[4] =
        new online_preliminary::COnlinePreliminaryMember();                                          // new(0x14)
    m_pad[5] =
        new online_preliminary::COnlinePreliminaryMemberMatchList();                                 // new(0x20)
    m_pad[6] = new Redeem_Item::CRedeemItem();    // new(0x10)
    m_pad[7] = new fair_pvp::CFairPvPScore();      // new(0x5c)

    // slot8 +0x20：operator new(0x158) → memset 全块 → CMissionList_Charac
    void* mem8 = ::operator new(0x158);
    memset(mem8, 0, 0x158);
    CMissionList_Charac* mission = new (mem8) CMissionList_Charac();
    m_pad[8] = mission;

    m_pad[9] = new CExpandEquipslot();                    // new(0x89c)
    m_pad[10] = new CItemMakingSkill();                    // new(0x20)
    m_pad[11] = new CQuestShop();                                // new(0x48)
    m_pad[12] = new CKillMonsterInfo();                    // new(0x50)
    m_pad[13] = new TOD_UserState();                          // new(0x20)
    m_pad[14] = new CTitleBook();                                // new(0x6b58)
    m_pad[15] = new CAchievement();                            // new(0x28)
    m_pad[16] = new CItemDictionary();  // new(0x1390)；CItemDictionary 现真继承 charac_expand::CData
    m_pad[17] = new CBoosterGage();                            // new(0xc)
    m_pad[18] = new BestClearTime();                          // new(0x24)
    m_pad[19] = new CCharacInformNotice();              // new(0x18)
