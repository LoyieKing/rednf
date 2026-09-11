// ORIG 0x08114dcc（InitEventManager.md 反汇编逐槽核对）：按槽位映射堆构造 50
// 个事件对象（new + C1 + 写槽），成功后调 LoadEventFromDB() 并透传其返回值；
// 任何构造异常经 catch 直接返回 0（ORIG catch @0x08115965 只 begin/end_catch，
// 不回收已构造对象）。GLOBAL 键控槽（21/24/35/36/39/50/58/86/87/91/102/119/
// 158）在 ORIG 为静态单例 getter 返回指针，析构同样 delete——语义等价堆构造，
// 统一 new。槽位 = 偏移/4，映射如下：
//   1 CUnlimitFatigueEvent 2 CMaxFatigueFactorEvent 3 CExpDoubleEvent
//   4 CCoinEventPerDay 5 CCoinEventOnCharCreate 6 CLeadingChannelEvent
//   7 CItemDropRatioEvent 8 CPCRoomBurningEvent 33 CGuildWarEvent
//   10 CPCRoomFatigueEvent 12 CReformingDanjinEvent 13 CCoinRefillEvent
//   15 CBurningFatigueEvent 16 CClearRewardCardEvent 17 CCeraShopBonusItemEvent
//   18 CTournamentPvPEvent 19 CGoldCardBlankItemEvent 29 CPcRoomCardBlankItemEvent
//   30 CPowerWarEvent 21 CCollectArchieveEventLog 22 CPCRoomWorldDropEvent
//   24 CPartyExpBonusEvent 32 CStabToDeathEvent 9 CSchoolMatchEvent
//   34 CAutoMarketConditionsControlEvent 35 CVendingMachineBonusEvent
//   36 CBurnigGoldMonsterEvent 38 CNoNeedGoldOnGuildCreateEvent
//   39 CDeathTowerWinPointEvent 40 CNotApplyBalkeunEvent 41 CCharacterDayEvent
//   49 CRestrictCharacCreationEvent 50 CReduceUpgradeItemPay 51 COnTimeEvent
//   52 CBreakAwayPreventEvent 53 CPowerWarVictoriousEvent 58 CSecretShopEvent
//   86 CFatigueAttendance 87 CWeekendBonusEvent 113 CConditionEvent
//   91 CUXGameLogEvent 95 BlueMarbleDungeonEvent 102 CStopOverlabExpEvent
//   119 CEventAdvanceAltarOpen 116 CAradRyosikaEvent 155 Arad_MomijiEvent
//   158 LevelupSupportEvent 159 CEventStayTime 160 CEventCreateDnf
//   165 EventGiveMeBox
// §6.2 巨件核销：P167 153/153 [0缺0多]；gap 927→927。
// [round5 归零] 原残余尾部 7 条（缺 `xor $0x1,%eax` 段 + 两臂块序）。真因 =
// 「汇聚变量」：ORIG 的 if 在 **try 体内**、两臂写 4 字节 `mov $0x0/%0x1,%ebx`，
// catch 体同写 `mov $0x0,%ebx` 并 fall-through 到 0x08115977 `mov %ebx,%eax`
// ⇒ 变量为 **register int**（非 bool 局部：不占槽、不物化 movb），函数返回类型
// 为 **int**（尾块无 bool 归一化 setne，P9/P159）。改法：`register int result;`
// 裸声明（零发射）+ `if (!LoadEventFromDB()) result = 0; else result = 1;` 移入
// try 内 + `return result;`；同步 CGuildServerProxy.h 返回类型 bool→int
// （mangled 不含返回类型、全仓无调用点 ⇒ 零 ABI 风险）。实测 927=927（AE 归位，
// TU diff 3→2）。
int CEventManager::InitEventManager()
{
    // b28d-solo-CEventManager: [N82/N80 封顶] §6.2 三件套已验——①gap=0（927=927）；
    // ②P167 调用点差集 153/153 CLEAR（0 缺 0 多）；③残差=全帧槽号平移 + cMyTrace
    // 变参保活顺序（P227 全表达式链已试）。ORIG 0x08114dcc。
    // b28d-solo 本轮复核：归一化后唯一实质差异 = 尾部 7 条（LoadEventFromDB 调用点
    // 的 `xor $0x1/test/je` bool 物化 + __cxa_begin/end_catch 块相对序，N104/P292；
    // ①`if(call())return true;return false;` 形 gap=0 但缺 xor 段，②否定形 928 条
    // 且 catch 块序反劣，取 ①）。双后端：4.4.7 DIFF / 4.4.6 后端 DIFF（后端免疫）。
    // [round5] 汇聚变量：裸 register 声明（无初值 ⇒ 零发射；ORIG 头部无 ebx 初始化）。
    register int result;
    try
    {
        m_events[1]  = new CUnlimitFatigueEvent;
        m_events[2]  = new CMaxFatigueFactorEvent;
        m_events[3]  = new CExpDoubleEvent;
        m_events[4]  = new CCoinEventPerDay;
        m_events[5]  = new CCoinEventOnCharCreate;
        m_events[6]  = new CLeadingChannelEvent;
        m_events[7]  = new CItemDropRatioEvent;
        m_events[8]  = new CPCRoomBurningEvent;
        m_events[33] = new CGuildWarEvent;
        m_events[10] = new CPCRoomFatigueEvent;
        m_events[12] = new CReformingDanjinEvent;
        m_events[13] = new CCoinRefillEvent;
        m_events[15] = new CBurningFatigueEvent;
        m_events[16] = new CClearRewardCardEvent;
        m_events[17] = new CCeraShopBonusItemEvent;
        m_events[18] = new CTournamentPvPEvent;
        m_events[19] = new CGoldCardBlankItemEvent;
        m_events[29] = new CPcRoomCardBlankItemEvent;
        m_events[30] = new CPowerWarEvent;
        m_events[21] = new CCollectArchieveEventLog;
        m_events[22] = new CPCRoomWorldDropEvent;
        m_events[24] = new CPartyExpBonusEvent;
        m_events[32] = new CStabToDeathEvent;
        m_events[9]  = new CSchoolMatchEvent;
        m_events[34] = new CAutoMarketConditionsControlEvent;
        m_events[35] = new CVendingMachineBonusEvent;
        m_events[36] = new CBurnigGoldMonsterEvent;
        m_events[38] = new CNoNeedGoldOnGuildCreateEvent;
        m_events[39] = new CDeathTowerWinPointEvent;
        m_events[40] = new CNotApplyBalkeunEvent;
        m_events[41] = new CCharacterDayEvent;
        m_events[49] = new CRestrictCharacCreationEvent;
        m_events[50] = new CReduceUpgradeItemPay;
        m_events[51] = new COnTimeEvent;
        m_events[52] = new CBreakAwayPreventEvent;
        m_events[53] = new CPowerWarVictoriousEvent;
        m_events[58] = new CSecretShopEvent;
        m_events[86] = new CFatigueAttendance;
        m_events[87] = new CWeekendBonusEvent;
        m_events[113] = new CConditionEvent;
        m_events[91] = new CUXGameLogEvent;
        m_events[95] = new BlueMarbleDungeonEvent;
        m_events[102] = new CStopOverlabExpEvent;
        m_events[119] = new CEventAdvanceAltarOpen;
        m_events[116] = new CAradRyosikaEvent;
        m_events[155] = new Arad_MomijiEvent;
        m_events[158] = new LevelupSupportEvent;
        m_events[159] = new CEventStayTime;
        m_events[160] = new CEventCreateDnf;
        m_events[165] = new EventGiveMeBox;
        // [round5 复测] ORIG 0x08115945..0x08115997 实证：if 检查在 **try 体内**
        // 末尾（`call LoadEventFromDB; xor $0x1,%eax; test %al,%al; je <mov $1
        // 块>; mov $0x0,%ebx; jmp 0xbab; <mov $0x1,%ebx>`），catch 体同写
        // `mov $0x0,%ebx` 并 fall-through 到 0xbab `mov %ebx,%eax` ⇒ 汇聚变量是
        // **register int**（两臂/尾块均 32 位 `mov $imm,%ebx`，非 bool 的 movb、
        // 也无槽）。前轮 `bool ok` 具名形（923 条）正是踩在「bool 局部必占槽」
        // （P158/§4.0）上，改用裸 register 声明即消槽。
        if (!LoadEventFromDB())
            result = 0;
        else
            result = 1;
    }
    catch (...)
    {
        result = 0;
    }
    return result;
}