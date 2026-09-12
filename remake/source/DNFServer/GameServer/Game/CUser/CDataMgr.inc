// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0832a778 | _ZN13charac_expand8CDataMgrC1Ev
// 2026-09-11 CuA（P5 父泳道点名「EXTRA 3×placement new/memset/delete」复核）：
//   p167fix 的 EXTRA `_ZnwjPv`/`memset`/`_ZdlPvS_` ×3 与 MISSING `_ZdlPv` 均**非多余结构** ——
//   ORIG 0x0832a781..0x0832a7b8 把 `operator new(size,ptr)` **内联**并把手写常量 `memset`
//   内联成 `rep stos`；4.4 两者都走 out-of-line call。我方源码已是最小单表达式
//   （下行三处 `new (memset(::operator new(N),0,N)) T()`，无额外指针具名/无显式析构）
//   ⇒ 属 N78b 跨版本内联差，无源码可改。
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
    // [R11a04 2026-09-12 ④§16.3 多余物化移除] ORIG 0x0832a8e1 段
    //   `movl $0x10,(%esp); call _Znwj; mov %eax,%ebx; mov %ebx,%eax; mov %eax,(%esp);
    //    call _ZN11Redeem_Item11CRedeemItemC1Ev`（**无任何前置清零**；同函数其余
    //   17 处 `new T` 亦无清零，仅 slot0/slot1/slot8 的 `new (memset(...))` 有）
    //   ⇒ 原码为 default-init（**去括号**）：我方原写 `new ...()` 触发 CWG434 value-init
    //   前置零初始化（4 条 `movl/movb $0x0` @我方 .o 0x1f4/0x1fa/0x1fe/0x205）⇒ 删。
    m_pad[6] = new Redeem_Item::CRedeemItem;    // new(0x10)
    m_pad[7] = new fair_pvp::CFairPvPScore();      // new(0x5c)

    // slot8 +0x20：operator new(0x158) → memset 全块 → CMissionList_Charac
    m_pad[8] = new (memset(::operator new(0x158), 0, 0x158)) CMissionList_Charac();

    m_pad[9] = new CExpandEquipslot();                    // new(0x89c)
    m_pad[10] = new CItemMakingSkill();                    // new(0x20)
    m_pad[11] = new CQuestShop();                                // new(0x48)
    // [R8a20 2026-09-12 ④] 构造顺序按 ORIG 复刻：ORIG 0x0832aa56 段先构 TOD_UserState
    //   （0x20B，存 +0x34 = m_pad[13]，0x0832aa92 `mov %edx,0x34(%eax)`）再构
    //   CKillMonsterInfo（0x50B，存 +0x30 = m_pad[12]，0x0832aace `mov %edx,0x30(%eax)`）；
    //   我方原按 m_pad[12]→m_pad[13] 书写（反序）⇒ 两条 new 调用链次序与 ORIG 互换。
    //   槽位/尺寸（+0x30=0x50 kill、+0x34=0x20 TOD）两侧逐值一致，仅语句序修正。
    m_pad[13] = new TOD_UserState();                          // new(0x20)
    m_pad[12] = new CKillMonsterInfo();                    // new(0x50)
    m_pad[14] = new CTitleBook();                                // new(0x6b58)
    m_pad[15] = new CAchievement();                            // new(0x28)
    m_pad[16] = new CItemDictionary();  // new(0x1390)；CItemDictionary 现真继承 charac_expand::CData
    m_pad[17] = new CBoosterGage();                            // new(0xc)
    m_pad[18] = new BestClearTime();                          // new(0x24)
    m_pad[19] = new CCharacInformNotice();              // new(0x18)
}