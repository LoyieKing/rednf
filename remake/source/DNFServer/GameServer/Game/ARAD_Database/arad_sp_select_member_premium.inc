int LOGIN_QUERY::arad_sp_select_member_premium(
    SIG_LOGIN_DATA* data, DB_Login::tagDNFEventEntry& entry,
    std::vector<int>& removeList, std::vector<int> removeScratch)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x412, "%s", "Database null!!");
        return -1;
    }

    if (db->select(
            "seLect pre_type,unix_timestamp(service_start),unix_timestamp(service_end) "
            "from member_premium where m_id=%d and event_id=%d and service_end>now() "
            "and server_id in(0,%d) order by service_end desc limit 1",
            data->m_accId, entry.m_eventId,
            entry.m_serverId) != 0)
    {
        return -1;
    }

    if (db->get_n_rows() == 0)
    {
        // 无记录 → 把事件 id 计入第4参 scratch 表（ORIG 仅 push param_4）
        (void)removeList;
        removeScratch.push_back(entry.m_eventId);
        return 0;
    }

    int slot = 0;
    // [round5 Main-B轴 实测负例·P196 红线] 曾试 `int row` + `(int)get_n_rows()`：改后该比较
    //   变有符号 `jle`，而 ORIG 是 `call get_n_rows; cmp -0xc(%ebp),%eax; seta %al; test %al,%al;
    //   jne 818cd68`（**seta = 无符号**）⇒ 违反 P196（禁止把无符号序改有符号），已回退。
    //   真正的残差是「ORIG 物化 `seta;test;jne` vs 我方直接 `jbe`」，非符号性；row 保持 unsigned。
    for (unsigned int row = 0;
         row < static_cast<unsigned int>(db->get_n_rows()) && db->fetch();
         ++row)
    {
        int preType = 0;
        int startSec = 0;
        int endSec = 0;
        if (!db->get_int(0, preType))
        {
            // P221/P227：ORIG 槽序 -0x44/-0x34/-0x24 与 4.4 具名序反向，且
            //   `data->m_accId` 在 ctor 前物化 %ebx 跨调用保活 ⇒ 全表达式临时链。
            cMyTrace(
                __PRETTY_FUNCTION__,
                0x432, 5)("DB_Login::GetPremiumInfo, get(1) ERROR m_id=%u",
                          data->m_accId);
            return -1;
        }
        if (!db->get_int(1, startSec))
        {
            cMyTrace(
                __PRETTY_FUNCTION__,
                0x438, 5)("DB_Login::GetPremiumInfo, get(2) ERROR m_id=%u",
                          data->m_accId);
            return -1;
        }
        if (!db->get_int(2, endSec))
        {
            cMyTrace(
                __PRETTY_FUNCTION__,
                0x43e, 5)("DB_Login::GetPremiumInfo, get(3) ERROR m_id=%u",
                          data->m_accId);
            return -1;
        }
        if (slot > 0x69)
        {
            break;
        }

        // premium 元素基址 = sig + (slot+0x229)*0x10；字段位 [+8 pre_type]
        // [+0xc start][+0x10 end][+0x14 event id]【ABI 镜像】
        // P183：ORIG 每个字段写都重发完整 (slot+0x229)*0x10 地址链（9/8 条/处），
        //   缓存 `prem` 引用会塌成 4 条/处（gap 12 根因）⇒ 逐字段全表达式展开；
        //   写入顺序照 ORIG：m_flag(+0x14) → m_type(+8) → start(+0xc) → end(+0x10)。
        (&data->m_premiums[slot])->m_flag = entry.m_eventId;
        (&data->m_premiums[slot])->m_type = static_cast<unsigned int>(preType);
        (&data->m_premiums[slot])->m_startTime = static_cast<unsigned int>(startSec);
        (&data->m_premiums[slot])->m_endTime = static_cast<unsigned int>(endSec);
        ++slot;

        // 计数 ushort@0x293a（m_restrictedGoodsCount）与 m_id 数组（+0x293c 起 int 表）：
        // ORIG 读 count 一次（%eax）复用索引与 +1 写回（P0 无具名槽）⇒ 后置 ++。
        data->m_restrictedGoods[data->m_restrictedGoodsCount++] = entry.m_eventId;
    }
    return 0;
