// ============================================================================
// DB_LoadEtc::LoadAccountFirstLogin @0x08405c6e —— 首登事件位图装载。
// bitset 位操作映射 std::bitset<32>::set/reset(pos, true) ORIG 外联符号
// （084506e4 set / 0845071e reset）。
// ============================================================================
// [b28d-solo-DB_LoadEtc] NEAR（N7 封顶）：ORIG 0x08405c6e。
// 残差（不可翻，N7/4.4.7）：间接虚调寄存器角色互换——ORIG
//   `mov (%eax),%edx; mov -0x14(%ebp),%eax; mov %eax,(%esp); call *%edx`
//   vs 4.4.7 `mov (%eax),%eax; mov -0x14(%ebp),%edx; …; call *%eax`
//   （P230 补：4.4.6 后端同 ORIG 用 %edx，本 TU 未路由 446 表 ⇒ 4.4.7 恒 %eax；改路由属 Main）。
// 已修：首段链式去 evMng 槽、三处 get_convert 重发（P48）、nRows 正极性 !=0 直落（P220）、
//   set_query 内联实参与 NumberToString（P259）。助记符序列全等。
bool DB_LoadEtc::LoadAccountFirstLogin(SIG_LOAD_ETC* sig, int eventCode)
{
    // ORIG 0x08405c75：首段 is_eventing 为链式直调（无 evMng 具名槽，P0 槽律）。
    if (!reinterpret_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())->is_eventing(eventCode))
    {
        return false;
    }
    EventClassify::CEventScriptMng* evMng =
        reinterpret_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng());
    EventClassify::CEventEntity* entity = evMng->get_event_entity(eventCode);
    if (entity == 0)
    {
        return false;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    // ORIG vtable+0x20 虚调用 get_db_table_name → &m_dbTableName（std::string 视图）
    // 经 toTString(std::string const&) 取表名；accStr 无具名槽（ebx 跨调用保活）。
    db->set_query("seLect m_id from %s where m_id=%s",
                  toTString(entity->get_db_table_name()),
                  NumberToString(sig->m_accId, 0));

    if (!db->exec(true))
    {
        // ORIG 0x08405d5d/0x08405da7/0x08405dd8：三处各自重发 get_convert 调用（P48 无 CSE）。
        sig->m_bitset.set((unsigned int)
            evMng->get_convert_to_user_confirmflag(eventCode), true);
        return false;
    }
    int nRows = db->get_n_rows();                            // ORIG -0xc 具名槽
    // ORIG 0x08405d9e：cmpl $0,-0xc; je <后置 reset 块> ⇒ 正极性 !=0 的 set 块直落。
    if (nRows != 0)
    {
        sig->m_bitset.set((unsigned int)
            evMng->get_convert_to_user_confirmflag(eventCode), true);  // ORIG bitset::set(Ejb)
    }
    else
    {
        sig->m_bitset.reset((unsigned int)
            evMng->get_convert_to_user_confirmflag(eventCode));  // ORIG bitset::reset(Ej)
    }
    return true;
}