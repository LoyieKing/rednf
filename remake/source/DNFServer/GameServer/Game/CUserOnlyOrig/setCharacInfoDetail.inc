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
    m_characList[slot].m_field22 = src->m_field22;                        // growType
    if (src->m_field22 == 7) {                                    // 学徒 → 固定节点
        m_characList[slot].m_field22 = 2;
        m_characList[slot].m_field24 = 7;
    }
    if (src->m_field22 == 8) {                                    // 二段成长互换
        unsigned char tmp = src->m_field22;
        m_characList[slot].m_field22 = src->m_field23;
        m_characList[slot].m_field25 = static_cast<unsigned char>(tmp);
    }
    if (src->m_field22 == 0xc || src->m_field22 == 0xd) {
        m_characList[slot].m_field22 = 0xb;
    }
    m_characList[slot].m_field23 = 2;
    m_characList[slot].m_field26 = src->m_field24;         // job
    m_characList[slot].m_level = static_cast<short>(src->m_field25);
    m_characList[slot].m_field2b = src->m_field28;
    m_characList[slot].m_field29.m_growthType = src->m_field27;
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
        static_cast<int>(m_characList[slot].m_field26), static_cast<int>(m_characList[slot].m_level),
        static_cast<int>(static_cast<signed char>(m_characList[slot].m_field29.m_growthType << 4) >> 4),
        static_cast<int>(static_cast<signed char>(m_characList[slot].m_field29.m_growthType * 2) >> 5));
    m_characList[slot].m_field33 = src->m_field2c;
    m_characList[slot].m_field35 = src->m_field2e;
    m_characList[slot].m_field37 = src->m_field30;
    {
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(2);
        if (ev != 0) {
            if (!ev->IsEventing(0)) {
                m_characList[slot].m_field35 = static_cast<unsigned short>(DEFAULT_MAX_FATIGUE);
            }
            else {
                int factor = reinterpret_cast<CMaxFatigueFactorEvent*>(ev)->GetMaxFatigueFactor();
                m_characList[slot].m_field35 =
                    static_cast<unsigned short>(factor * DEFAULT_MAX_FATIGUE / 100);
            }
        }
    }
    {
        // ORIG 事件 0x57 无空检查直调（本域多函数同款）；+0x51c 为
        // CDataManager 疲劳附加脚本参数（CUser.cpp 2399-2416 同型，尚未具名化）
        CEventBase* ev57 = GlobalData::s_event_manager->GetRepeatEvent(0x57);
        if (ev57->IsEventing(0)) {
            unsigned short base = m_characList[slot].m_field35;
            m_characList[slot].m_field35 = static_cast<unsigned short>(
                base + *reinterpret_cast<const unsigned short*>(
                           reinterpret_cast<const char*>(G_CDataManager()) + 0x51c));
        }
    }
    if (static_cast<short>(src->m_field32) < 0) {          // 疲劳电池错误分支
        m_characList[slot].m_field4f = 0;
        short badVal = static_cast<short>(src->m_field32);
        cMyTrace t(__PRETTY_FUNCTION__,
                   0xf99, 0);
        t("FATIGUE BATTERY Error : Char No(%d) %d",
          src->m_characNo, static_cast<int>(badVal));
    }
    else {
        m_characList[slot].m_field4f = src->m_field32;
    }
    m_characList[slot].m_resetDwordDf4 = 0;
    m_characList[slot].m_resetDwordDf8 = 0;
    m_characList[slot].m_field7b = src->m_field4d;
    {
        unsigned int lastTick = m_characList[slot].m_field7b;
        cMyTrace t(__PRETTY_FUNCTION__,
                   0xfb0, 0);
        t("CharLastPlayTick char(%d) t(%d)", characNo, lastTick);
    }
    m_characList[slot].m_field7f = src->m_unkB2.m_field118;
    m_characList[slot].m_guildId = src->m_guildId;
    m_characList[slot].m_fielddc2 = src->m_field5a;
    m_characList[slot].m_field77 = src->m_field5b;
    m_characList[slot].m_field79 = src->m_field5d;
    if (src->m_field5f != 0) {
        m_characList[slot].m_fielde22 = src->m_characNo;
    }
    reinterpret_cast<CommonTime*>(m_characList[slot].m_delReqTime)->SetCreateTime(
        src->m_field34);
    m_characList[slot].m_field4d = src->m_unkB2.m_fieldb3;
    m_characList[slot].m_field51 = src->m_unkB2.m_fieldb5;
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
    m_characList[slot].m_fielde49 = src->m_unkB2.m_fieldd5;
    m_characList[slot].m_fielde4d = src->m_unkB2.m_fieldd9;
    m_characList[slot].m_fielde51 = src->m_unkB2.m_fielddd;
    m_characList[slot].m_fielde55 = src->m_unkB2.m_fielde1;
    m_characList[slot].m_fielde59 = src->m_unkB2.m_fielde5;
    m_characList[slot].m_fielde5d = src->m_unkB2.m_fielde9;
    m_characList[slot].m_fielde45 = src->m_unkB2.m_fielded;
    m_characList[slot].m_fielde68 = src->m_unkB2.m_fieldf1;
    m_characList[slot].m_fielde70 = static_cast<unsigned char>(src->m_unkB2.m_fieldf5);
    m_characList[slot].m_fielde71 = src->m_unkB2.m_fieldf6;
    m_characList[slot].m_fielde79 = src->m_unkB2.m_fieldfa;
    m_characList[slot].m_fielde81 = src->m_unkB2.m_field10a;
    m_characList[slot].m_fielde7d = src->m_unkB2.m_field106;
    m_characList[slot].m_field121f = src->m_unkB2.m_field10e;
    m_characList[slot].m_field1221 = src->m_unkB2.m_field110;
    m_characList[slot].m_field1225 = src->m_unkB2.m_field114;
    m_characList[slot].m_field1239 = static_cast<unsigned char>(src->m_unkB2.m_field11e);
    m_characList[slot].m_field123e = static_cast<unsigned char>(src->m_unkB2.m_field11f);
    m_characList[slot].m_field123f = static_cast<unsigned char>(src->m_unkB2.m_field120);
    m_characList[slot].m_field1241 = (src->m_unkB2.m_field121 != 0);
    m_characList[slot].m_field126c = src->m_unkB2.m_field122;
    m_characList[slot].m_field126e = static_cast<unsigned char>(src->m_unkB2.m_b124);
    m_characList[slot].m_field126f = src->m_unkB2.m_i125;
    m_characList[slot].m_field1275 = static_cast<unsigned char>(src->m_unkB2.m_b129);
    m_characList[slot].m_field1276 = static_cast<unsigned char>(src->m_unkB2.m_b12a);
    m_characList[slot].m_field1277 = static_cast<unsigned char>(src->m_unkB2.m_b12b);
    m_characList[slot].m_field127a = static_cast<unsigned char>(src->m_unkB2.m_b12d);
    m_characList[slot].m_field1278 = static_cast<unsigned char>(src->m_unkB2.m_b12c);
    m_characList[slot].m_field1280 = static_cast<unsigned char>(src->m_unkB2.m_b12f);
    m_characList[slot].m_field12ad = src->m_unkB2.m_i130;
    return true;
