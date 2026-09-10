charac_expand::CDataMgr::CDataMgr()
{
    // slot0 +0x00：ORIG 0x0832a781..0x0832a7b8 `movl $0x30,(%esp); call operator new;
    //   mov %eax,%esi`（**无具名指针槽**，mem 保活 %esi）→ `rep stos`（0xb dword +
    //   尾 word，4.1 对常量 memset 的内联形，N78b 不可翻案）→ `mov %ebx,(%esp);
    //   call CCharacExpertJob::CCharacExpertJob()` ⇒ 指针不再落具名槽：以
    //   `new (memset(::operator new(N), 0, N)) T()` 单表达式表达（P0/P183）。
    m_pad[0] = new (memset(::operator new(0x30), 0, 0x30)) expert_job::CCharacExpertJob();

    // slot1 +0x04：ORIG 0x0832a7c6 `mov %eax,-0x20(%ebp)`（此件 ORIG 有槽）→ 同上
    m_pad[1] = new (memset(::operator new(0x2c), 0, 0x2c)) CCharacBloodDungeon();

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
    m_pad[8] = new (memset(::operator new(0x158), 0, 0x158)) CMissionList_Charac();

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
