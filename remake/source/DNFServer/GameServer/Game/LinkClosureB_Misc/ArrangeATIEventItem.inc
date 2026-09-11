// ArrangeATIEventItem —— ORIG T 0x083fa1d3 (size 0x287)
// 行收集：item_check==0 且 item_server_id==serverId 一条；
// avata_server_id==serverId 时 avata_check1==0 / avata_check2==0 各一条。
// [b28d-solo-LinkClosureB_Misc] N 封顶：语义/调用点已闭合（p167 24/24 CLEAR）。
// 已修：__PRETTY_FUNCTION__（P139）、dead local limit=10（P232）、P227 全表达式
// 临时链、P164/P283 for 底部条件 + unsigned 尾测 seta、P171 删 count<10 防御、
// 存储序 [0],[2],[3],[1]。残余 = P209（4.4 unsigned 形参比较恒载寄存器 +1/分支）
// + P311/N81 族（4.4 对 rows[count][1..2] 常量列下标折 scaled，4.1 恒 shl+lea+sub 链）。
// [F2e-Singles2 C05② 复核 2026-09-10] 两处 cmp 差（ORIG `mov -0x40/-0x54(%ebp),%eax;
//   cmp 0xc(%ebp),%eax` mem 直比）**比较对象是 unsigned 形参 serverId**（0xc(%ebp)）⇒
//   按 C05② 边界表 ❌ 形参子型，P209/N18b 封顶（改形参会动 mangled，禁）。主 gap 来源
//   仍是 P311 数组链（our 290 vs ORIG 307，3 处 shl$4+lea 链被 4.4 折 scaled）。
// [P8S3 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
// [L3FLinkClosureQuickParty 2026-09-11] §6.5.1：ORIG PF 串第一形参类型名 =
// memberIdentificationNumber_t（全局 typedef，见聚合器顶部）。
void EventATI::ArrangeATIEventItem(memberIdentificationNumber_t mid, unsigned int serverId)
{
    // ORIG 函数头 `movl $0xa,-0x1c` 为未被读的 dead int 局部（P232）。
    int limit = 10;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "seLect id,cera,cera_check,item_no,item_server_id,item_charac_no,item_check,"
        "avata_no1,avata_no2,avata_server_id,avata_charac_no,avata_check1,avata_check2 "
        "from event_ati_info where m_id=%s and occ_check!=0 limit %d",
        NumberToString(mid, 0), 10);
    if (!db->exec(true))
    {
        // P227 全表达式临时链：NumberToString 跨 ctor 以 %ebx 保活。
        cMyTrace(__PRETTY_FUNCTION__, 0xa1, 5)(
            "%s, exec ERROR m_id=%s", __PRETTY_FUNCTION__,
            NumberToString(mid, 0));
        return;
    }

    // [L3FLinkClosureQuickParty 2026-09-11] P311 反向（同族先例 WongWorkEvents_A
    // ArrangeFCComboEventItem）：ORIG 每列寻址是两步 `shl $0x4; lea -0x8(%ebp);
    // lea (%ebx,%edx,1); sub $K; mov`（4.1 不折 scaled）；二维 `unsigned int[10][4]`
    // 会被 4.4 折成 `mov %eax,-0x100(%ebp,%edx,8)`（每处少 2 条）。struct 行数组
    // 复现 ORIG 形态。
    struct Entry
    {
        unsigned int m_id;         // [0] +0x00 id
        int          m_characNo;   // [1] +0x04 item_charac_no / avata_charac_no
        int          m_itemNo;     // [2] +0x08 item_no / avata_no
        int          m_flag;       // [3] +0x0c 恒 1
    };
    Entry rows[10];
    int count = 0;
    // P283/P164：ORIG for 底部条件布局 + unsigned 尾测（seta）。
    for (unsigned int scanned = 0;
         scanned < static_cast<unsigned int>(db->get_n_rows()); ++scanned)
    {
        if (!db->fetch())
            break;
        unsigned int rowId = 0; int cera = 0; int ceraCheck = 0; int itemNo = 0;
        int itemServerId = 0; int itemCharacNo = 0; int itemCheck = 0;
        int avataNo1 = 0; int avataNo2 = 0; int avataServerId = 0;
        int avataCharacNo = 0; int avataCheck1 = 0; int avataCheck2 = 0;
        db->get_int(0, rowId);
        db->get_int(1, cera);
        db->get_int(2, ceraCheck);
        db->get_int(3, itemNo);
        db->get_int(4, itemServerId);
        db->get_int(5, itemCharacNo);
        db->get_int(6, itemCheck);
        db->get_int(7, avataNo1);
        db->get_int(8, avataNo2);
        db->get_int(9, avataServerId);
        db->get_int(0xa, avataCharacNo);
        db->get_int(0xb, avataCheck1);
        db->get_int(0xc, avataCheck2);

        // P171：ORIG 无 count<10 防御（照抄越界语义），存储序 [0],[2],[3],[1]。
        if (itemCheck == 0 && itemServerId == serverId)
        {
            rows[count].m_id = static_cast<unsigned int>(rowId);
            rows[count].m_itemNo = itemNo;
            rows[count].m_flag = 1;
            rows[count].m_characNo = itemCharacNo;
            ++count;
        }
        if (avataServerId == serverId)
        {
            if (avataCheck1 == 0)
            {
                rows[count].m_id = static_cast<unsigned int>(rowId);
                rows[count].m_itemNo = avataNo1;
                rows[count].m_flag = 1;
                rows[count].m_characNo = avataCharacNo;
                ++count;
            }
            if (avataCheck2 == 0)
            {
                rows[count].m_id = static_cast<unsigned int>(rowId);
                rows[count].m_itemNo = avataNo2;
                rows[count].m_flag = 1;
                rows[count].m_characNo = avataCharacNo;
                ++count;
            }
        }
    }

    for (int i = 0; i < count; ++i)
    {
        UpdateEventATIInfo(rows[i].m_id, rows[i].m_itemNo);
        EventCommon::InsertEventItem2EventTable(rows[i].m_characNo, rows[i].m_itemNo,
                                                rows[i].m_flag, 2);
    }
}