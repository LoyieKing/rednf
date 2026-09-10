bool DB_LoadEtc::LoadTODUserState(SIG_LOAD_ETC* sig)
{
    static const int handleIdx = 2;    // ORIG _ZZN10DB_LoadEtc16LoadTODUserState…E9handleIdx
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect unix_timestamp(first_layer_start_date),"
                  "today_enter_count,last_clear_layer,enter_count_by_week,"
                  "unix_timestamp(m_date),unix_timestamp(last_clear_date) from"
                  " charac_tower_despair where charac_no = %u", sig->m_characNo);
    // ORIG 0x08406f04 起：exec/fetch/AffectedRowCount==1 三守卫**正极性**嵌套
    // （bool 直测 test/je + `or/xor/sete`），任一失败共享函数尾 return true
    // （P214①/P59；负极性形多 xor/mov$1/jmp 块）。
    if (db->exec(true))
    {
    if (db->fetch())
    {
    if (db->getAffectedRowCount() == 1)
    {
    int col = 0;                          // ORIG 08406f85 movl $0,-0x10
    bool ok = false;                      // ORIG 08406f8c movb $0,-0x9 每步 get 同槽保存
    ok = db->get_int(col++, sig->m_todFirstLayerStartDate);  // +0xd71c
    if (!ok)
    {                                                        // first_layer_start_date
        // P227 全表达式临时链（ORIG 0x08406fbd：ctor 前 m_characNo 装 %ebx 跨 ctor 保活；
        // 具名 tt 形使 trace 槽方向反 + 值在 ctor 后才载）。
        cMyTrace(__PRETTY_FUNCTION__, 0x1b77, 5)(
            "TOD : select from charac_tower_despair charac_no=%u",
            sig->m_characNo);
        return false;
    }
    ok = db->get_int(col++, sig->m_todTodayEnterCount);      // +0xd720
    if (!ok)
    {                                                        // today_enter_count
        cMyTrace(__PRETTY_FUNCTION__, 0x1b7f, 5)(
            "TOD select from charac_tower_despair charac_no=%u",
            sig->m_characNo);
        return false;
    }
    unsigned short layerVal;             // ORIG 无预清（get_ushort 直写 -0x76）
    ok = db->get_ushort(col++, layerVal);                    // last_clear_layer(ushort)
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b88, 5)(
            "[@TOD] select from charac_tower_despair charac_no=%d",
            sig->m_characNo);
        return false;
    }
    TOD_Layer layer(layerVal);                               // ORIG TOD_Layer C1(T)@0x085fe7b4
    // [b28d-solo-DB_LoadEtc] P211 同型化：ORIG 0x08407114 为 base-first
    // （`mov 0xc(%ebp),%eax` 先于 `movzwl -0x78(%ebp),%edx`）；直写具名字段
    // 4.4 恒 RHS-first（探针 /tmp/dble_probe/p1.cpp f1）。具名视图 cast（同探针
    // f7）复现 ORIG 的 base-first + 单 disp `mov %dx,0xd724(%eax)`。
    struct TODLayerSlotView
    {
        char m_pad[0xd724];
        unsigned short m_layer;
    };
    reinterpret_cast<TODLayerSlotView*>(sig)->m_layer = layer.m_layer;  // +0xd724（movw）
    ok = db->get_short(col++,
                       *reinterpret_cast<short*>(&sig->m_todEnterCountByWeek));
    if (!ok)
    {                                                        // enter_count_by_week
        cMyTrace(__PRETTY_FUNCTION__, 0x1b92, 5)(
            "[@TOD] select from charac_tower_despair charac_no=%d",
            sig->m_characNo);
        return false;
    }
    ok = db->get_int(col++, sig->m_todDate);                 // +0xd728
    if (!ok)
    {                                                        // m_date
        cMyTrace(__PRETTY_FUNCTION__, 0x1b9a, 5)(
            "[@TOD] select from charac_tower_despair charac_no=%d",
            sig->m_characNo);
        return false;
    }
    ok = db->get_int(col++, sig->m_todLastClearDate);        // +0xd72c（ORIG addl $1 实证）
    if (!ok)
    {                                                        // last_clear_date
        cMyTrace(__PRETTY_FUNCTION__, 0x1ba2, 5)(
            "[@TOD] select from charac_tower_despair charac_no=%d",
            sig->m_characNo);
        return false;
    }
    }
    }
    }
    return true;
