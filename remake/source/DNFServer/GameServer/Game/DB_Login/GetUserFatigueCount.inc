// [S216_DB_Login2] verdict=NEAR ndiff=8 gap=0（N7：entity vtable+0x20 间接 call
// 目标 4.4.7 恒 %eax / ORIG %edx，且 occDay 装载时刻差 1 位；4.4.6 后端可翻但属
// Main 路由裁决。指令数 118=118、p167 CLEAR）。
// S216 本轮 DIFF→NEAR：NumberToString 内联进 set_query 实参 ⇒ occDay
// `movzbl -0x11(%ebp),%esi` 装载时刻回到 ORIG 位；余 N7 间接 call 目标寄存器
// （4.4.7 恒 %eax vs ORIG %edx，4.4.6 后端可翻，属 Main 路由裁决）。
int DB_Login::GetUserFatigueCount(SIG_LOGIN_DATA* sig)
{
    // [b28c-L9] 残差 6 = 全为跨版本寄存器/调度差（N7）：
    // ① entity vtable+0x20 间接 call：ORIG funcptr→%edx/实参→%eax，4.4 恒互换；
    // ② `movzbl -0x11(%ebp),%esi`（occDay）装载时刻相对 G_CDataManager 调用点差 1 位。
    // 指令数 118=118、助记符序列全等 ⇒ NEAR 封顶。
    // ORIG：无 mng 具名局部，每处重新求值链式表达式（P48 无 CSE）
    if (!(static_cast<EventClassify::CEventScriptMng*>(
              G_CDataManager()->get_event_script_mng()))->is_eventing(0x56))
    {
        return 0;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    sig->m_fatigueValue = 0;                             // +0x3faa
    sig->m_fatigueKind = 0;                              // +0x3fa9

    EventClassify::CEventEntity* entity = 0;
    entity = static_cast<EventClassify::CEventEntity*>(
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())->get_event_entity(0x56));
    if (entity == 0)
    {
        return 0;
    }
    unsigned char occDay = static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng())->get_today_time_day();

    // ORIG vtable+0x20 槽：取事件表名（std::string），经 toTString 恒等转 const char*；
    // 结果不落槽，直接进实参位（ORIG +0x0f4 `mov %eax,0x8(%esp)`）。
    typedef std::string* (*GetTableFn)(void*);
    db->set_query("seLect fatigue from %s where m_id=%s and occ_day = %d",
                  toTString(*((*reinterpret_cast<GetTableFn*>(
                      *reinterpret_cast<void**>(entity) + 0x20))(entity))),
                  NumberToString(sig->m_accId, 0), (unsigned int)occDay);
    if (!db->exec(true))
    {
        return 0;
    }
    int rows = db->get_n_rows();
    if (rows > 0)
    {
        short fatigue = 0;
        bool ok = db->fetch();
        if (!ok)
        {
            return 0;
        }
        ok = db->get_short(0, fatigue);
        if (!ok)
        {
            return 0;
        }
        sig->m_fatigueValue = fatigue;
        sig->m_fatigueKind = occDay;                     // ORIG param[0x3fa9] = local_15(day)
    }
    return 1;
}