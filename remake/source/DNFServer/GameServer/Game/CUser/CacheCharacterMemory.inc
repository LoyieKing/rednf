bool CUser::CacheCharacterMemory()   // b28c-L5 SIGFIX（P174）
{
    if (getCurCharacR() == 0)
        return 0;
    if (get_state() < 3)
        return 0;

    // ORIG 0x0864c8..：characNo = *((uint*)getCurCharacR()) 直读首字段 m_characNo
    //（P167：再次 getCurCharacR，勿经 getCurCharacNo() 访问器）。
    unsigned int characNo = (unsigned int)getCurCharacR()->m_characNo;
    int characIdx = get_charac_idx(characNo);
    if (characIdx < 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x113c, 0)
            ("get_charac_idx() return = %d", characIdx);
        return 0;
    }

    CACHE_CHARACTER_TYPE* mem = exchange_server::AcquireCharacterMemory((int)characNo);
    if (mem == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1150, 0)
            ("AcquireCharacterMemory() return = NULL, charac_no(%d)", (int)characNo);
        return 0;
    }
    GetRentInfo(mem->m_inventory);
    GetCharacInfo(&mem->m_charac);
    GetInventoryInfo(&mem->m_inventory);
    getCurCharacQuestR()->get_quest(&mem->m_quest);
    GetTicket(reinterpret_cast<SIG_LOAD_TICKET*>(mem->m_ticket));
    GetDungeonClear(reinterpret_cast<SIG_LOAD_DUNGEON_CLEAR*>(mem->m_dungeonClear));
    GetBlackListCount(reinterpret_cast<SIG_REQ_BLACK_COUNT*>(mem->m_blackList));
    APSystem::CDBAndExchangeServerProc::GetActionPointFromMemory(
        *this, mem->m_actionPoint);
    CExpandEquipslot* expand = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xc);
    // 真实虚调用 CExpandEquipslot::getData(char*) @ vtable+8（CExpandEquipslot.h
    // 权威 slot2）。§4.5 禁 typedef 槽指针直调。
    expand->getData((char*)&mem->m_expand);
    advancealtar::DBAndExchangeServerProc::getFromMemory(
        *this, mem->m_altar);
    GetEtcInfo(&mem->m_etc);

    // 三个子对象内字段直拷（ORIG 报告偏移）
    // m+0x1fd2c=etc.m_fieldC / m+0x686=charac.m_tagCharacNo / m+0x70b=inv.m_field8
    //（均 movl 4B，CacheCharacterMemory.md 0x08670d3x 区）。
    mem->m_etc.m_fieldC = mem->m_charac.m_tagCharacNo;
    mem->m_inventory.m_field8 = mem->m_etc.m_fieldC;
    memcpy(mem->m_memberFlag, m_pad8d0fd, 0xc);

    if (exchange_server::CacheCharacterMemory((int)characNo, mem))
    {
        SetLastLoginChannelNo(G_CEnvironment()->get_channel_no());
        SetLastLoginCharacNo((int)characNo);
        return 1;
    }
    exchange_server::FreeCharacterMemory(mem);
    cMyTrace(__PRETTY_FUNCTION__, 0x1190, 0)
        ("CacheCharacterMemory() return = false");
    return 0;
