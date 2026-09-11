// ORIG 0x0818cc74：member_premium 首条生效记录 → SIG premium 表格区。
// verdict=DIFF 202/197（gap 12→5）。P167 调用点集 16/16 CLEAR。已修三处：
//   ①`prem` 引用缓存去 P183 ⇒ 四字段各自全表达式地址链（ORIG 9/8 条/处）；
//   ②cnt 具名槽去 P0 ⇒ `m_restrictedGoods[m_restrictedGoodsCount++]`（ORIG 读一次复用）；
//   ③三处 cMyTrace 具名对象改全表达式临时链 P221/P227 ⇒ 槽落 ORIG 的
//     -0x44/-0x34/-0x24 且 `data->m_accId` 在 ctor 前物化 %ebx 保活。
// 残余 N（5 条）：4 处 ORIG `add %ecx,%edx; add $K,%edx; mov %eax,(%edx)` vs 4.4
//   `mov %edx,K(%ecx,%eax,1)`（N4/N107 寻址折叠 + 值/基址寄存器互换）+ 循环条件
//   `&&` 短路物化侧（ORIG 物化第一条件 seta vs 4.4 物化第二条件，P197④/N 族；
//   德摩根形实测不变）。语义等价。
// [S5-2026-09-11 复攻] §6.5 串：SQL 串逐字节命中 ORIG（`seLect pre_type,…`）；
//   prett 第4参拼写还原为 `vecPremiumRemoveInfo_t`（ORIG 串实证，见 .rodata 比对）；
//   余 `static ` 前缀 + 第3参 `IntVector&` 需改 .h（class→namespace；全局 typedef
//   IntVector 本仓缺失）⇒ 上游报告，不在本 TU 写域。gap −1 / ndiff 17 维持（N 族：
//   ORIG `seta;test;jne` vs 4.4 `jbe` 循环条件物化侧）。
int LOGIN_QUERY::arad_sp_select_member_premium(
    SIG_LOGIN_DATA* data, DB_Login::tagDNFEventEntry& entry,
    IntVector& removeList, vecPremiumRemoveInfo_t removeScratch)
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
    // [G20 2026-09-12 P-G7a 试形] ORIG 结构实证（0x818cd55..818cf9c）：
    //   体首 818cd68 `call fetch; xor $1,%al; test %al,%al; jne 818cf9c(出)`；
    //   尾 818cf81 `call get_n_rows; cmp -0xc(%ebp),%eax; seta %al; test %al,%al; jne 818cd68`
    //   （**seta = 无符号**，P196 红线：row 必须 unsigned）
    //   ⇒ 原码 = `while(row<n){ if(!fetch()) break; ...; ++row; }`。
    //   旧 for(...; row<n && db->fetch(); ...) 形被 4.4 把第二条件折进条件区并物化 $1/$0
    //   （结构错位 + 尾部无 seta/test）；改 while 后条件位物化（P-G7a）与体首 break 对齐 ORIG。
    unsigned int row = 0;
    while (row < static_cast<unsigned int>(db->get_n_rows()))
    {
        if (!db->fetch())
        {
            break;
        }
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
        ++row;
    }
    return 0;
}