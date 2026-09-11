// ArrangeMuzEventItem —— ORIG T 0x083fb16a (size 0x240)
// [b28d-solo-LinkClosureB_Misc] N 封顶：同 ArrangeATIEventItem（p167 CLEAR）。
// 已修：__PRETTY_FUNCTION__、dead local limit=10、P227 临时链、for 底部条件、
// 存储序 [0],[2],[3],[1]。残余 = P209 + P311 数组常量列下标 scaled 折叠。
// [F2e-Singles2 C05② 复核 2026-09-10] 同上：`cmp 0xc(%ebp),%eax`（serverId 形参）⇒
//   N18b/P209 封顶；our 184 vs ORIG 194 gap +10 主体为 P311 scaled 折叠。
// [P8S3 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
// [L3FLinkClosureQuickParty 2026-09-11] §6.5.1：ORIG PF 串第一形参类型名 =
// memberIdentificationNumber_t（全局 typedef，见聚合器顶部）。
void EventMuz::ArrangeMuzEventItem(memberIdentificationNumber_t mid, unsigned int serverId)
{
    // [b28d-solo-LinkClosureB_Misc] ORIG 函数头 `movl $0xa,-0x1c` 为未被读的
    // dead int 局部（P232/P31 帧槽对齐）；kFunc 局部已删（P118/P139 用
    // __PRETTY_FUNCTION__ 生成 _ZZ...E19__PRETTY_FUNCTION__ 符号）。
    int limit = 10;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "seLect id,item_server_id,item_charac_no,item_no,item_check,item_cnt "
        "from event_muz_entry where m_id=%s and occ_check!=0 limit %d",
        NumberToString(mid, 0), 10);
    if (!db->exec(true))
    {
        // P227：全表达式临时链（构造即调用）令 NumberToString 在 ctor 前求值
        // 并以 %ebx 跨 ctor 保活（ORIG `call NumberToString; mov %eax,%ebx`）。
        cMyTrace(__PRETTY_FUNCTION__, 0x1ce, 5)(
            "%s, exec ERROR m_id=%s", __PRETTY_FUNCTION__,
            NumberToString(mid, 0));
        return;
    }

    // [L3FLinkClosureQuickParty 2026-09-11] P311 反向（同 ArrangeATIEventItem）：
    // struct 行数组复现 ORIG 每列两步寻址（`shl $0x4; lea -0x8(%ebp); lea; sub $0xd0/
    // $0xd4/$0xd8`）；二维数组会被 4.4 折 scaled。
    struct Entry
    {
        unsigned int m_id;         // [0] +0x00 id
        int          m_characNo;   // [1] +0x04 item_charac_no
        int          m_itemNo;     // [2] +0x08 item_no
        int          m_itemCnt;    // [3] +0x0c item_cnt
    };
    Entry rows[10];
    int count = 0;
    // P283/P164：ORIG 为 for 底部条件布局（fetch 在体内、get_n_rows 在尾条件），
    // 且 scanned 为 unsigned（尾测 seta）。
    for (unsigned int scanned = 0;
         scanned < static_cast<unsigned int>(db->get_n_rows()); ++scanned)
    {
        if (!db->fetch())
            break;
        unsigned int rowId = 0; int itemServerId = 0; int itemCharacNo = 0;
        int itemNo = 0; int itemCheck = 0; int itemCnt = 0;
        db->get_int(0, rowId);
        db->get_int(1, itemServerId);
        db->get_int(2, itemCharacNo);
        db->get_int(3, itemNo);
        db->get_int(4, itemCheck);
        db->get_int(5, itemCnt);

        if (itemCheck == 0 && itemServerId == serverId)
        {
            // ORIG 存储序 [0],[2],[3],[1]（-0x30/-0x3c/-0x44/-0x38 实证）。
            rows[count].m_id = static_cast<unsigned int>(rowId);
            rows[count].m_itemNo = itemNo;
            rows[count].m_itemCnt = itemCnt;
            rows[count].m_characNo = itemCharacNo;
            ++count;
        }
    }

    for (int i = 0; i < count; ++i)
    {
        UpdateEventMuzInfo(rows[i].m_id, rows[i].m_itemNo);
        EventCommon::InsertEventItem2EventTable(rows[i].m_characNo, rows[i].m_itemNo,
                                                rows[i].m_itemCnt, 5);
    }
}