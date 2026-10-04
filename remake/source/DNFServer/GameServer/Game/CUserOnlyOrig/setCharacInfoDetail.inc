// ---------------------------------------------------------------------------
// CUser::setCharacInfoDetail(int slot, unsigned int characNo, CHARAC_DATA* src)
// T 0x0864ac1a sz0xfe9 —— 报告：setCharacInfoDetail.md（## 汇编+## 反编译 C 全文）。
// slot/characNo 校验 → CHARAC_DATA 全字段搬运（成长型特化 7/8/0xc/0xd）→
// initCharacStat → 疲劳上限事件(2/0x57) → 疲劳电池错误分支 → LastPlayTick 等。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x0864ac1a：p167=M0/E1:_ZNSt6vectorI12_Charac_infoSaIS0_EEixEj，gap=26；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0864ac1a：N82 封顶——帧/槽差（p167 M0/E1，gap 26）。
bool CUser::setCharacInfoDetail(int slot, unsigned int characNo, CHARAC_DATA* src)
{
    if (slot < 0 || slot >= static_cast<int>(m_characList.size())) {
        return false;
    }
    if (m_characList[slot].m_characNo != static_cast<int>(characNo)) {
        return false;
    }
    // 2026-09-04 §4.3：s 字节视图→CHARAC_DATA 具名字段（SigTypes_C.h union 字段 by CUserMain）
    m_characList[slot].m_characNo = src->m_characNo;
    if (G_GameWorld()->GetChannelType() != 7) {
        strncpy(m_characList[slot].m_name, src->m_name, 0x1e);
    }
    m_characList[slot].m_vill = src->m_field22;                        // growType
    if (src->m_field22 == 7) {                                    // 学徒 → 固定节点
        m_characList[slot].m_vill = 2;
        m_characList[slot].m_villSpecial = 7;
    }
    if (src->m_field22 == 8) {                                    // 二段成长互换
        unsigned char tmp = src->m_field22;
        m_characList[slot].m_vill = src->m_field23;
        m_characList[slot].m_villPrev = static_cast<unsigned char>(tmp);
    }
    if (src->m_field22 == 0xc || src->m_field22 == 0xd) {
        m_characList[slot].m_vill = 0xb;
    }
    m_characList[slot].m_villFixed = 2;
    m_characList[slot].m_job = src->m_field24;         // job
    m_characList[slot].m_level = static_cast<short>(src->m_field25);
    m_characList[slot].m_exp = src->m_field28;
    m_characList[slot].m_growthType.m_growthType = src->m_field27;
    m_characList[slot].m_stamina = src->m_unkB2.m_fieldb2;
    // [L6 判据②/§8.3 + L31CharacPad 成员化] 0x88..0xd9 由 17 个 pad 字段合并为具名
    // 子对象 `_Additioal_info m_additioalInfo`（offsetof 断言锁定 +0x88）⇒ 视图 cast
    // 反模式消除，逐字段具名直访问（宽度/偏移不变）。
    m_characList[slot].m_additioalInfo.m_field00 = src->m_add.m_field00;
    m_characList[slot].m_additioalInfo.m_field04 = src->m_add.m_field04;
    m_characList[slot].m_additioalInfo.m_field08 = src->m_add.m_field08;
    m_characList[slot].m_additioalInfo.m_field0a = src->m_add.m_field0a;
    m_characList[slot].m_additioalInfo.m_field0c = src->m_add.m_field0c;
    m_characList[slot].m_additioalInfo.m_field0e = src->m_add.m_field0e;
    memcpy(&m_characList[slot].m_additioalInfo.m_field10[0], &src->m_add.m_field10[0], 8);
    memcpy(&m_characList[slot].m_additioalInfo.m_field18[0], &src->m_add.m_field18[0], 0x22);
    m_characList[slot].m_additioalInfo.m_field3a = src->m_add.m_field3a;
    m_characList[slot].m_additioalInfo.m_field3e = src->m_add.m_field3e;
    m_characList[slot].m_additioalInfo.m_field40 = src->m_add.m_field40;
    m_characList[slot].m_additioalInfo.m_field42 = src->m_add.m_field42;
    m_characList[slot].m_additioalInfo.m_field46 = src->m_add.m_field46;
    m_characList[slot].m_additioalInfo.m_field48 = src->m_add.m_field48;
    m_characList[slot].m_additioalInfo.m_field4a = src->m_add.m_field4a;
    m_characList[slot].m_additioalInfo.m_field4c = src->m_add.m_field4c;
    m_characList[slot].m_additioalInfo.m_field4e = src->m_add.m_field4e;
    global::tools::InitializeCharacStat::initCharacStat(
        m_characList[slot].m_additioalInfo,
        static_cast<unsigned int>(m_characList[slot].m_characNo),
        static_cast<int>(m_characList[slot].m_job), static_cast<int>(m_characList[slot].m_level),
        // [H6 2026-09-11 单旋钮·位域窄化复现] ORIG 0x0864b192 `shl $0x4,%eax; sar $0x4,%al;
        // movsbl` = m_sub（bit0-3，signed char:4）；0x0864b16b `add %eax,%eax; sar $0x5,%al;
        // movsbl` = m_grow（bit4-6，signed char:3）。旧的 32 位移位手写形值等价但 4.4 产
        // 32 位 `sar`；STGrowthType.h 已有该位域视角 ⇒ 直读位域成员。
        static_cast<int>(m_characList[slot].m_growthType.m_sub),
        static_cast<int>(m_characList[slot].m_growthType.m_grow));
    m_characList[slot].m_fatigue = src->m_field2c;
    m_characList[slot].m_maxFatigue = src->m_field2e;
    m_characList[slot].m_fatigueGrownUpBuff = src->m_field30;
    {
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(2);
        if (ev != 0) {
            if (!ev->IsEventing(0)) {
                m_characList[slot].m_maxFatigue = static_cast<unsigned short>(DEFAULT_MAX_FATIGUE);
            }
            else {
                int factor = reinterpret_cast<CMaxFatigueFactorEvent*>(ev)->GetMaxFatigueFactor();
                m_characList[slot].m_maxFatigue =
                    static_cast<unsigned short>(factor * DEFAULT_MAX_FATIGUE / 100);
            }
        }
    }
    {
        // ORIG 事件 0x57 无空检查直调（本域多函数同款）；+0x51c 为
        // CDataManager 疲劳附加脚本参数（CUser.cpp 2399-2416 同型，尚未具名化）
        CEventBase* ev57 = GlobalData::s_event_manager->GetRepeatEvent(0x57);
        if (ev57->IsEventing(0)) {
            // [CUserOnlyOrig-Lane 2026-09-11 P167 EXTRA op[] 修复] ORIG 0x0864b38c-0x0864b3a6：
            // 单次 `call vector<_Charac_info>::operator[]` + `mov %eax,%ebx` 后
            // `movzwl 0x35(%ebx),%esi; …; mov %ax,0x35(%ebx)`（读/写同一元素指针，无 base 槽）；
            // 旧形「读 base + 写回」两次完整下标 ⇒ EXTRA op[]×1（ORIG 92 vs our 93）。
            _Charac_info* pci = &m_characList[slot];
            pci->m_maxFatigue = static_cast<unsigned short>(
                pci->m_maxFatigue + G_CDataManager()->m_serverParameter.m_weekendBouns4b4.m_bonusFatiguePercent);
        }
    }
    if (static_cast<short>(src->m_field32) < 0) {          // 疲劳电池错误分支
        m_characList[slot].m_fatigueBattery = 0;
        short badVal = static_cast<short>(src->m_field32);
        cMyTrace t(__PRETTY_FUNCTION__,
                   0xf99, 0);
        t("FATIGUE BATTERY Error : Char No(%d) %d",
          src->m_characNo, static_cast<int>(badVal));
    }
    else {
        m_characList[slot].m_fatigueBattery = src->m_field32;
    }
    m_characList[slot].m_resetDwordDf4 = 0;
    m_characList[slot].m_resetDwordDf8 = 0;
    m_characList[slot].m_lastPlayTick = src->m_field4d;
    {
        unsigned int lastTick = m_characList[slot].m_lastPlayTick;
        cMyTrace t(__PRETTY_FUNCTION__,
                   0xfb0, 0);
        t("CharLastPlayTick char(%d) t(%d)", characNo, lastTick);
    }
    m_characList[slot].m_lastPlayTickPowerWar = src->m_unkB2.m_field118;
    m_characList[slot].m_guildId = src->m_guildId;
    m_characList[slot].m_guildCreateRight = src->m_field5a;
    m_characList[slot].m_premiumFatigue = src->m_field5b;
    m_characList[slot].m_maxPremiumFatigue = src->m_field5d;
    if (src->m_field5f != 0) {
        m_characList[slot].m_memberkey = src->m_characNo;
    }
    reinterpret_cast<CommonTime*>(m_characList[slot].m_delReqTime)->SetCreateTime(
        src->m_field34);
    m_characList[slot].m_usedFatigue = src->m_unkB2.m_fieldb3;
    m_characList[slot].m_tutorialFlag = src->m_unkB2.m_fieldb5;
    memcpy(m_characList[slot].m_tradeRestrictData, src->m_unkB2.m_fieldb9, 0x10);
    reinterpret_cast<CReliablePerson*>(m_characList[slot].m_reliablePerson)->InitTradeGold(
        src->m_unkB2.m_fieldc9,
        src->m_unkB2.m_fieldcd,
        src->m_unkB2.m_fieldd1);
    {
        unsigned int useGold = src->m_unkB2.m_fieldd1;
        cMyTrace t(__PRETTY_FUNCTION__,
                   0xfd7, 0);
        t("TRADE_RESTIRCTION charac_name(%s), use_gold(%u)", m_characList[slot].m_name, useGold);
    }
    m_characList[slot].m_chaosPoint = src->m_unkB2.m_fieldd5;
    m_characList[slot].m_chaosExp = src->m_unkB2.m_fieldd9;
    m_characList[slot].m_chaosModeCount = src->m_unkB2.m_fielddd;
    m_characList[slot].m_chaosKillCount = src->m_unkB2.m_fielde1;
    m_characList[slot].m_chaosDieCount = src->m_unkB2.m_fielde5;
    m_characList[slot].m_chaosDieTime = src->m_unkB2.m_fielde9;
    m_characList[slot].m_assaultCount = src->m_unkB2.m_fielded;
    m_characList[slot].m_chaosKillTime = src->m_unkB2.m_fieldf1;
    m_characList[slot].m_opencoin = static_cast<unsigned char>(src->m_unkB2.m_fieldf5);
    m_characList[slot].m_luckPoint = src->m_unkB2.m_fieldf6;
    m_characList[slot].m_dungeonPlayCount = src->m_unkB2.m_fieldfa;
    m_characList[slot].m_expertJobExp = src->m_unkB2.m_field10a;
    m_characList[slot].m_expertJobType = src->m_unkB2.m_field106;
    m_characList[slot].m_powerWarPoint = src->m_unkB2.m_field10e;
    m_characList[slot].m_powerWarAssaultCount = src->m_unkB2.m_field110;
    m_characList[slot].m_powerWarAssaultVictoryCount = src->m_unkB2.m_field114;
    m_characList[slot].m_skillTreeIndex = static_cast<unsigned char>(src->m_unkB2.m_field11e);
    m_characList[slot].m_exEquipSlotStat = static_cast<unsigned char>(src->m_unkB2.m_field11f);
    m_characList[slot].m_eventCharacterGrowtype = static_cast<unsigned char>(src->m_unkB2.m_field120);
    m_characList[slot].m_memberDungeonFlag = (src->m_unkB2.m_field121 != 0);
    m_characList[slot].m_memberBonusFatigue = src->m_unkB2.m_field122;
    m_characList[slot].m_guildSecede = static_cast<unsigned char>(src->m_unkB2.m_b124);
    m_characList[slot].m_lastPlayDungeonIndex = src->m_unkB2.m_i125;
    m_characList[slot].m_equipslotFlag = static_cast<unsigned char>(src->m_unkB2.m_b129);
    m_characList[slot].m_channelEquipslotSwitch = static_cast<unsigned char>(src->m_unkB2.m_b12a);
    m_characList[slot].m_expandEquipslotSwitch = static_cast<unsigned char>(src->m_unkB2.m_b12b);
    m_characList[slot].m_useBringUpMaterialTotal = static_cast<unsigned char>(src->m_unkB2.m_b12d);
    m_characList[slot].m_bringUpRewardFlag = static_cast<unsigned char>(src->m_unkB2.m_b12c);
    m_characList[slot].m_growthPowerReward = static_cast<unsigned char>(src->m_unkB2.m_b12f);
    m_characList[slot].m_chaosResponTime = src->m_unkB2.m_i130;
    return true;
}