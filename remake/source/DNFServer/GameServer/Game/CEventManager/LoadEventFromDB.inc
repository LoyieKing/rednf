bool CEventManager::LoadEventFromDB()
{
    // b28d-solo-CEventManager: [N 封顶] ORIG 0x081159b6 实证——fetch 在循环体内
    // （`ok = db->fetch(); if (!ok) break;`，非 for 条件）；复用同一 bool ok 局部
    // （P102，ORIG 槽 -0x25 反复赋值）；get_int/get_ushort 直接写出参
    // （type 无初始化、param 成员直接传址）；type/param 传值 TriggerEventStart。
    // 143=143（助记符序列全等）、p167 14/14 CLEAR。残差 = 聚合对象优先低槽
    // （N82附）：ORIG trace -0x38 / db -0x24 / ok -0x25 vs 4.4 trace -0x48 /
    // db -0x28 / ok -0x21（rows -0x20、i -0x1c、type -0x4c、param -0x50 两侧一致）；
    // 4.1 按生命周期穿插 vs 4.4 类对象沉底 + bool 紧贴下邻标量，声明序/纯 register
    // 均不可翻（P182/N95）。变参寄存器已对齐 %edi/%esi/%ebx（P87）。
    // b28d-solo 双后端实测：4.4.7 NEAR / 4.4.6 后端 NEAR（槽平移非后端敏感，
    // 与 N7 不同族）。
    // [round5 复测] 槽位序：ORIG db=-0x24 / ok=-0x25（ok 紧贴 db 之下）而 4.4
    // 旧形 db=-0x28 / ok=-0x21（ok 反落在 db 之上）⇒ 试把 `bool ok;` 声明提前
    // 到 db 之前（4.4 对函数头声明按「先声明深」分配，P309/P331）。
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect event_type, parameter1, parameter2 from dnf_event_log where "
        "end_time = 0 and server_id in (0,%d) and unix_timestamp() >= start_time "
        "order by start_time",
        G_CEnvironment()->m_serverEnvir.m_serverGroup);
    cMyTrace traceEnter(__PRETTY_FUNCTION__, 0x296, 9, true, true);
    traceEnter("[!] Load Event From DB");
    ok = db->exec(true);
    if (!ok)
        return false;
    const int rows = db->get_n_rows();
    for (int i = 0; i < rows; ++i)
    {
        ok = db->fetch();
        if (!ok)
            break;
        int type;
        Word_Param param;
        ok = db->get_int(0, type);
        if (!ok)
            return false;
        ok = db->get_ushort(1, param.m_duration);
        if (!ok)
            return false;
        ok = db->get_ushort(2, param.m_field2);
        if (!ok)
            return false;
        if (!TriggerEventStart(type, param))
            return false;
        // P227 全表达式临时链：ORIG 0x08115af2 在 ctor 前右起求 varargs 并以
        // %edi/%esi/%ebx 跨 ctor 保活（4.4 具名对象形恒用 caller-saved）。
        cMyTrace(__PRETTY_FUNCTION__, 0x305, 9, true, true)(
            "\t- Event Code(%d) , Parameter1(%d), Parameter2(%d)",
            type, (int)param.m_duration, (int)param.m_field2);
    }
    return true;
