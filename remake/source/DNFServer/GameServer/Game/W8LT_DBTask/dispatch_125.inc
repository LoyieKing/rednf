bool DB_AvatarChangeStat::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    DB_AvatarChangeStatIn* sig = reinterpret_cast<DB_AvatarChangeStatIn*>(
        stream->GetOutBuffer<SIG_AVATAR_CHANGE_STAT>());
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate user_items set stat=%d,slot=%d where ui_id=%d and charac_no=%d",
        sig->m_itId, (unsigned int)sig->m_slot, sig->m_uiId, sig->m_characNo);
    if (db->exec(true) != 1)
        return 0;
    if (sig->m_itId == 0)
    {
        db->set_query(
            "seLect slot,ui_id,it_id,unix_timestamp(reg_date),unix_timestamp(expire_date),ability_no,ipg_agency_no,jewel_socket,hidden_option,emblem_endurance from user_items where charac_no=%u and ui_id=%d",
            sig->m_characNo, sig->m_uiId);
        if (db->exec(true) != 1)
            return 0;
        if (db->fetch() != 1)
            return 0;
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xbe2), true);
        try
        {
            guard.operator*()->operator<<((int)0x36);
            guard.operator*()->operator<<((int)slot);
            // [b28d-solo] P103/P167：ORIG 调 operator->（ptEv），非 operator*。
            DB_AvatarChangeStatView* out =
                reinterpret_cast<DB_AvatarChangeStatView*>(
                    guard.operator->()->GetInBuffer<SIG_AVATAR_CHANGE_STAT>());
            int slotV = 0;            // col0 slot（回包 +0x08 byte）
            int uiIdV = 0;            // col1 ui_id（回包 +0x00）
            int itIdV = 0;            // col2 it_id（回包 +0x04）
            int abilityV = 0;         // col5 ability_no（回包 +0x09 byte）
            unsigned int regV = 0;    // col3 reg_date uint（回包 +0x0a）
            unsigned int expV = 0;    // col4 expire_date uint（回包 +0x0e）
            // ORIG 槽序：expV(-0x3c)/emblem(-0x5a..-0x3d)/hiddenV(-0x60)/
            // hiddenS(-0x62)/emblemEnd(-0x64)/zero(-0x10)；emblem 声明在
            // hiddenV 前（init() 调用在 get_str 后）。
            stAvatarEmblemInfo_t emblem;   // ORIG 槽 -0x5a
            int hiddenV = 0;          // col8 hidden_option int（回包 +0x2b short）
            int zero = 0;             // 回包 +0x2d 恒 0（ORIG movl 清零）
            if (!db->get_int(0, slotV))
                throw 0xbf2;
            if (!db->get_int(1, uiIdV))
                throw 0xbf3;
            if (!db->get_int(2, itIdV))
                throw 0xbf4;
            if (!db->get_uint(3, regV))
                throw 0xbf5;
            if (!db->get_uint(4, expV))
                throw 0xbf6;
            if (!db->get_int(5, abilityV))
                throw 0xbf7;
            if (!db->get_str(6, out->m_ipgAgencyNo, 0x18))
                throw 0xbf9;
            emblem.init();
            if (db->get_binary(7, &emblem, 0x1e))
            {
                if (!db->get_int(8, hiddenV))
                    throw 0xc00;
                short hiddenS = 0;    // col8 hidden_option short（回包 +0x4d）
                short emblemEnd = 0;  // col9 emblem_endurance（回包 +0x4f）
                if (!db->get_short(8, hiddenS))
                    throw 0xc07;
                if (!db->get_short(9, emblemEnd))
                    throw 0xc08;
                out->m_field4d = hiddenS;
                out->m_emblemEndurance = emblemEnd;
                out->m_slot = (unsigned char)slotV;
                out->m_uiId = uiIdV;
                out->m_itId = itIdV;
                out->m_regDate = (int)regV;
                out->m_expireDate = (int)expV;
                out->m_abilityNo = (unsigned char)abilityV;
                // ORIG 0x08400bd6(+0x578)（report dispatch.md）：emblem(stAvatarEmblemInfo_t
                // 0x1e) → m_jewelSocket(+0x2f) 为 8 条直拷（7 dword + 1 word），
                // 非 memcpy（同 SIG_AVATAR_INSERT::setJewelSocket 镜像法）。
                // [b28d-lane] 逐字面数组写形实测反劣（4.4 每 store 重算 base：
                // ndiff 40→92，多 15 add+15 lea）⇒ 维持结构赋值形，余 1 条
                // `add $0x2f` 基址推进为 N4/N18a 封顶。
                *reinterpret_cast<stAvatarEmblemInfo_t*>(out->m_jewelSocket) =
                    emblem;
                out->m_hiddenOption = (short)hiddenV;
                out->m_field2d = zero;
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            }
            else
            {
                return 0;
            }
        }
        catch (int)
        {
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
    }
    return 1;
