bool DB_LoadEtc::LoadUsedGiftFatigueInfo(SIG_LOAD_ETC* sig)
{
    sig->m_curCharacUsedGiftFatigueQuantity = 0;             // +0xc9b8 清零先

    EventClassify::CEventScriptMng* evMng =
        reinterpret_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng());
    // [b29-Lane21] ORIG 0x08406835 `cmpl $0,-0x14; je L1($1)` + 0x0840684e `xor $1;test;je
    //   L0($0)` + `test;je 主流程` ⇒ 原码是「`||` 短路 + 取反 + 无具名 bool」(P0：ORIG
    //   槽集无该 bool)；旧 P214① 正极性包裹形产 `test %al` 无 `xor` ⇒ DIFF。
    if (evMng == 0 || !evMng->is_eventing(0x65))
    {
        return true;
    }
    {
        EventClassify::CUsedFatigueGiveItem* entity =
            static_cast<EventClassify::CUsedFatigueGiveItem*>(
                evMng->get_event_entity(0x65));
        // ORIG 0x08406885：`movb $0,-0x15` 在 entity 存槽后、判空之前（声明点）。
        bool isCharacEvent = false;
        // 同第一段（ORIG 0x084068a1 `xor $1`）：`||` 短路 + 取反 + 早返。
        if (entity == 0 || !entity->getIsCharacEvent(isCharacEvent))
        {
            return true;
        }
        {
            MySQL* db = 0;
            // ORIG 0x084068d1：具名 std::string（_ZNSsC1ERKSs 到 -0x1c）+ 每分支各调 toTString
            // （p167 MISSING 指认 C1ERKSs/D1/toTString/_Unwind_Resume）。
            std::string tableName = entity->get_db_table_name();
            // ORIG 0x084068ea：`xor $1; je` 落 idx2 分支 ⇒ fall-through 为「非角色事件」idx9/m_id
            // 分支（P220 极性翻转；旧块序相反）。
            if (!isCharacEvent)
            {
                db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                       (ENUM_SERVER_GROUP)0);
                // ORIG 0x0840692a：NumberToString 结果留 %ebx 跨 toTString call 保活（P259 内联实参）。
                db->set_query("seLect fatigue_quantity from %s where m_id = %s",
                              toTString(tableName),
                              NumberToString(sig->m_accId, 0));
            }
            else
            {
                db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                       (ENUM_SERVER_GROUP)0);
                db->set_query("seLect fatigue_quantity from %s where charac_no = %u",
                              toTString(tableName), sig->m_characNo);
            }

            if (!db->exec(true))
            {
                return false;
            }
            // [b29-Lane21] ORIG 0x084069c4 `movl $0x0,-0x20(%ebp)` 紧贴 exec 之后
            // ⇒ quantity 声明在 get_n_rows 之前（先声明→更深槽）。
            unsigned int quantity = 0;
            // ORIG 0x084069c4..0x084069f6 真值门禁：get_n_rows()==0 → 返回 1
            // （`sete %al` 物化，P225）；fetch() 失败 → 返回 0。旧合并短路写法
            // `|| !db->fetch()` 返回 true 属真语义 bug（fetch 失败被当成功）。
            if (db->get_n_rows() == 0)
            {
                return true;
            }
            if (!db->fetch())
            {
                return false;
            }
            if (!db->get_uint(0, quantity))
            {
                return false;
            }
            sig->m_curCharacUsedGiftFatigueQuantity = quantity;
        }
    }
    return true;
