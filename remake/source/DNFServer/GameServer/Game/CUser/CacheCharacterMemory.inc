// ORIG T 0x0864c85a sz0x30b：把当前角色内存快照填入 CACHE_CHARACTER_TYPE 后交
// exchange_server 缓存。控制流/子对象偏移/错误路径逐指令核对 CacheCharacterMemory.md：
//   getCurCharacR()==0 -> false；get_state()<3 -> false；characNo=*curR(首字段)；
//   mem=exchange_server::AcquireCharacterMemory(characNo)（失败 trace + false）；
//   依次 GetRentInfo(+0x703)/GetCharacInfo(+0x0)/GetInventoryInfo(+0x703)/
//   quest(+0x15b4f)/ticket(+0x1d157)/dungeonClear(+0x1d2f3)/blacklist(+0x1eb03)/
//   actionPoint(+0x1eb13)/expand vtable 槽+8(+0x34884)/altar(+0x1f804)/etc(+0x1fd20)；
//   三个子对象内字段拷贝 + memcpy(+0x3f850, this+0x8d0fd, 0xc)；
//   CacheCharacterMemory 失败 -> FreeCharacterMemory + trace + false；成功置 lastLogin + true。
// b28d-lane5 N封顶: P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x0864c85a | _ZN5CUser20CacheCharacterMemoryEv
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
    // 真实虚调用 CExpandEquipslot::getData(char*) @ vtable+8（CExpandEquipslot.h
    // 权威 slot2）。§4.5 禁 typedef 槽指针直调。
    // 2026-09-11 CuA A组 P0/P183：ORIG 0x0864c9xx 该处**无具名指针槽**
    //   （`call GetCharacExpandDataR; mov (%eax),%edx; add $0x8,%edx; mov (%edx),%ecx;
    //   … mov %eax,(%esp); call *%ecx` 结果直接留 %eax）⇒ 我方旧具名 `expand`
    //   占 -0xc 槽，令 characNo/characIdx/mem/三处 trace 槽整体深移 4
    //   （-0x18/-0x14/-0x10/-0x48/-0x38 vs ORIG -0x14/-0x10/-0xc/-0x44/-0x34）
    //   ⇒ 链式内联回实参位。
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xc)->getData((char*)&mem->m_expand);
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
}