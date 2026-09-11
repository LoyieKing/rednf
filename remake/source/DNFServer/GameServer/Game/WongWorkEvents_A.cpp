// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 WongWorkEvents_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <string>
#include <vector>

#include "WongWorkEvents_A.h"
#include "MySQL.h"
#include "DBMgr.h"
#include "GlobalData.h"      // GlobalData::s_db_mgr / s_stream_pool / s_msgq_mgr
#include "GameTypes.h"       // cMyTrace
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "SigTypes.h"        // SIG_LOAD_EVENTITEM
#include "QuestClear.h"      // STClearedQuest（本 TU 产出其 ctor）

// ---- TU 局部外部依赖（全局符号，producer 归属其它 TU/批次）----
char* NumberToString(unsigned int value, int radix);          // _Z14NumberToStringji

namespace WongWork
{

// ---- TU 局部外部依赖（producer 归属其它 TU/批次）----

namespace DBCommon
{
unsigned int GetIdentity(MySQL* db);                          // _ZN8WongWork8DBCommon11GetIdentityEP5MySQL
}

// SIG_LOAD_EVENTITEM 字段视图（SigTypes.h 当前为 char m_pad[0x5c]，布局等价）。
struct SigLoadEventItemView
{
    unsigned int m_eventId;        // +0x00
    unsigned int m_characNo;       // +0x04
    int          m_itemCount;      // +0x08
    struct Entry                    // stride 0x10 @+0x0c
    {
        unsigned int m_dbId;
        unsigned int m_itemId;
        unsigned int m_stackCount;
        unsigned int m_eventCode;
    };
    Entry m_entries[5];
};
#ifdef WONGWORK_EVENT_STATIC_ASSERT_SIZEOF
WONGWORK_EVENT_STATIC_ASSERT_SIZEOF(SigLoadEventItemView, 0x5c);
#endif

#include "WongWorkEvents_A/GetDBHandle.inc"


#include "WongWorkEvents_A/UpdateEventAlttleInfo.inc"


#include "WongWorkEvents_A/CheckAlreadyGetAlttleItem.inc"


#include "WongWorkEvents_A/ArrangeAlttleEventItem.inc"
{
    const unsigned int MAX_ENTRIES = 10;   // local_24 = 10
    // [b28d-solo-WongWorkEvents_A] P311 反向：ORIG 每列两步寻址；struct 行
    // 数组避免 4.4 对 [1]/[2] 的 scaled 折叠（探针 arr.cpp f2）。
    struct AlttleEntry
    {
        unsigned int m_id;        // [0] mid
        int          m_characNo;  // [1]
        int          m_itemNo;    // [2]
        int          m_itemCnt;   // [3]
    };
    AlttleEntry entries[MAX_ENTRIES];

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "seLect item_server_id,item_charac_no,item_no,item_check,item_cnt from "
        "event_alttle_info where m_id=%s and occ_check!=0 limit %d",
        NumberToString(mid, 0), 10);
    if (!db->exec(true))
    {
        // [b28d-solo-WongWorkEvents_A] P227/P139：ORIG 0x083fb612 先
        // NumberToString 存 %ebx 跨 ctor 保活 + __PRETTY_FUNCTION__ 同址双用。
        cMyTrace(__PRETTY_FUNCTION__, 0x21b, 5)
            ("%s, exec ERROR m_id=%s", __PRETTY_FUNCTION__, NumberToString(mid, 0));
        return;
    }

    int outCount = 0;                       // -0x18（写满区计数）
    unsigned int row = 0;                   // -0x14（fetch 游标）
    // [b28d-solo-WongWorkEvents_A] ORIG 0x083fb67c 尾测 while + 体首
    // `call fetch; xor $1; test; jne exit`（P225 setcc 形）。
    while (row < (unsigned int)db->get_n_rows())
    {
        if (!db->fetch())
            break;
        // [b28d-solo-WongWorkEvents_A] ORIG 0x083fb697 `movl $0,-0x10(%ebp)`
        // 为循环体内声明、每轮清零且从不使用的 dead 局部（P171/P232 照抄；
        // 它把 db/outCount/row 与列局部整体下移 4 槽）。
        unsigned int unused = 0;
        (void)unused;
        unsigned int server_id = 0;         // col0
        int charac_no = 0;                  // col1
        unsigned int item_no = 0;           // col2
        int item_check = 0;                 // col3
        unsigned int item_cnt = 0;          // col4
        db->get_int(0, (int&)server_id);    // ORIG 全走 get_int(col,&int)
        db->get_int(1, charac_no);
        db->get_int(2, (int&)item_no);
        db->get_int(3, item_check);
        db->get_int(4, (int&)item_cnt);

        // ORIG 无 outCount<MAX 界检（查询 LIMIT 10 保证 ≤10 行，auStack_e8[40] 不外溢）
        if (item_check == 0 && server_id == serverId)
        {
            entries[outCount].m_id = mid;
            entries[outCount].m_itemNo = item_no;
            entries[outCount].m_itemCnt = item_cnt;
            entries[outCount].m_characNo = charac_no;
            ++outCount;
        }
        ++row;
    }

    for (int i = 0; i < outCount; ++i)
    {
        UpdateEventAlttleInfo(entries[i].m_id, entries[i].m_itemNo);
        EventCommon::InsertEventItem2EventTable(entries[i].m_characNo, entries[i].m_itemNo,
                                                entries[i].m_itemCnt, 6);
    }
}

#include "WongWorkEvents_A/UpdateEventSigongInfo.inc"


#include "WongWorkEvents_A/CheckAlreadyGetSigongItem.inc"


#include "WongWorkEvents_A/ArrangeSigongEventItem.inc"
{
    const unsigned int MAX_ENTRIES = 10;
    // [b28d-solo-WongWorkEvents_A] P311 反向：struct 行数组复现 ORIG 每列
    // 两步寻址（`shl $4; lea -0x8(%ebp); lea; sub $K`）。
    struct SigongEntry
    {
        unsigned int m_id;        // [0] id
        int          m_characNo;  // [1] itemN_charac_no
        int          m_itemNo;    // [2] itemN_no
        int          m_itemCnt;   // [3] 常量 1/200
    };
    SigongEntry entries[MAX_ENTRIES];

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "seLect id,item1_no,item1_server_id,item1_charac_no,item1_check,"
        "item2_no,item2_server_id,item2_charac_no,item2_check from "
        "event_sigong_info where m_id=%s and occ_check!=0 limit %d",
        NumberToString(mid, 0), 10);
    if (!db->exec(true))
    {
        // [b28d-solo-WongWorkEvents_A] P227/P139：ORIG 0x083fa8d2 先
        // NumberToString 存 %ebx 跨 ctor 保活 + __PRETTY_FUNCTION__ 同址双用。
        cMyTrace(__PRETTY_FUNCTION__, 0x123, 5)
            ("%s, exec ERROR m_id=%s", __PRETTY_FUNCTION__, NumberToString(mid, 0));
        return;
    }

    int outCount = 0;
    unsigned int row = 0;
    // [b28d-solo-WongWorkEvents_A] ORIG 0x083fa93c 尾测 while + 体首
    // `call fetch; xor $1; test; jne exit`（P225 setcc 形）。
    while (row < (unsigned int)db->get_n_rows())
    {
        if (!db->fetch())
            break;
        // [b28d-solo-WongWorkEvents_A] P311：ORIG 0x083fa957 对 rowBuf 的
        // 访问是 `lea -0x34(%ebp)` / `mov -0x54(%ebp,%eax,4)` 形（常量偏移
        // 折进 disp、变量索引留 scaled）。裸 `unsigned[11]`/局部 struct 视图
        // 在 4.4 下恒发 `lea -0x5c(%ebp); add $K` / `add $K,%eax`（+1 条/处）
        // ⇒ 改「每栏独立 2 元素数组 + 独立 id」形（声明序按 4.4 先声明浅，
        // 使 id 落 -0x34、cnts 落 -0x5c，与 ORIG 槽位一致）。
        unsigned int id;        // [10] -0x34
        int items[2];           // [8],[9]
        int servers[2];         // [6],[7]
        int characs[2];         // [4],[5]
        int checks[2];          // [2],[3]
        int cnts[2];            // [0],[1]
        // ORIG 初始化发射序 = id,[8],[9],[6],[7],[4],[5],[2],[3],[0],[1]。
        id = 0;
        items[0] = 0;             // item1_no
        items[1] = 0;             // item2_no
        servers[0] = 0;           // item1_server_id
        servers[1] = 0;           // item2_server_id
        characs[0] = 0;           // item1_charac_no
        characs[1] = 0;           // item2_charac_no
        checks[0] = 0;            // item1_check
        checks[1] = 0;            // item2_check
        cnts[0] = 1;              // ORIG: item1 常量数量
        cnts[1] = 200;            // ORIG: item2 常量数量

        db->get_int(0, id);            // id（ORIG 选 unsigned 重载）
        db->get_int(1, items[0]);      // item1_no
        db->get_int(2, servers[0]);    // item1_server_id
        db->get_int(3, characs[0]);    // item1_charac_no
        db->get_int(4, checks[0]);     // item1_check
        db->get_int(5, items[1]);      // item2_no
        db->get_int(6, servers[1]);    // item2_server_id
        db->get_int(7, characs[1]);    // item2_charac_no
        db->get_int(8, checks[1]);     // item2_check

        for (int i = 0; i <= 1; ++i)       // ORIG cmpl $0x1 setle
        {
            // [b28d-solo-WongWorkEvents_A] P140：ORIG 无 outCount<MAX 界检。
            if (checks[i] == 0 && servers[i] == serverId)
            {
                entries[outCount].m_id = id;
                entries[outCount].m_itemNo = items[i];
                entries[outCount].m_itemCnt = cnts[i];
                entries[outCount].m_characNo = characs[i];
                ++outCount;
            }
        }
        ++row;
    }

    for (int k = 0; k < outCount; ++k)
    {
        UpdateEventSigongInfo(entries[k].m_id, entries[k].m_itemNo);
        EventCommon::InsertEventItem2EventTable(entries[k].m_characNo, entries[k].m_itemNo,
                                                entries[k].m_itemCnt, 3);
    }
}

#include "WongWorkEvents_A/UpdateEventEverlandInfo.inc"


#include "WongWorkEvents_A/CheckAlreadyGetEverlandItem.inc"


#include "WongWorkEvents_A/ArrangeEverlandEventItem.inc"
{
    const unsigned int MAX_ENTRIES = 10;
    // [b28d-solo-WongWorkEvents_A] P311 反向：ORIG 0x083faf16 对每列均发
    // 两步寻址（`shl $4; lea -0x8(%ebp); lea; sub $K`）；`unsigned[10][4]`
    // 在 4.4 下 [1]/[2] 被折成 scaled（`shl $2; add $1; -0xe0(%ebp,%edx,4)`）。
    // 改 struct 行数组（成员访问恒两步形，探针 /tmp/b28d_ww_probe/arr.cpp f2）。
    struct EverlandEntry
    {
        unsigned int m_id;        // [0]
        int          m_characNo;  // [1]
        int          m_itemNo;    // [2] 恒 1
        int          m_coin;      // [3]
    };
    EverlandEntry entries[MAX_ENTRIES];

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "seLect id,server_id,charac_no,coin,coin_check from event_everland_info "
        "where m_id=%s and occ_check!=0 limit %d",
        NumberToString(mid, 0), 10);
    if (!db->exec(true))
    {
        // [b28d-solo-WongWorkEvents_A] P227/P139：ORIG 0x083faddd 先
        // NumberToString 存 %ebx 跨 cMyTrace ctor 保活（push %ebx/sub 0xf4），
        // 两处 %s 实参同址 `_ZZ…E19__PRETTY_FUNCTION__` ⇒ 全表达式临时链 +
        // __PRETTY_FUNCTION__ 宏。
        cMyTrace(__PRETTY_FUNCTION__, 0x182, 5)
            ("%s, exec ERROR m_id=%s", __PRETTY_FUNCTION__, NumberToString(mid, 0));
        return;
    }

    int outCount = 0;
    unsigned int row = 0;
    // [b23S3C] ORIG 0x083fad60 循环形 = 尾测 while：进入前 `movl $0x0,-0x10(%ebp);
    // jmp <tail>`，体首 `mov -0x18(%ebp),%eax; call fetch;
    // xor $0x1,%eax; test %al,%al; jne <exit>`（P194 xor 直测），尾 `call get_n_rows;
    // cmp -0x10(%ebp),%eax; seta %al; test %al,%al; jne <body>` 后才回体 ⇒ fetch 先于
    // 行数界判定（原 `while (row < n && fetch())` 短路序相反，丢体首 call）。
    while (row < (unsigned int)db->get_n_rows())
    {
        if (!db->fetch())
            break;
        unsigned int id = 0;               // col0
        int srv = 0;                       // col1
        int charac_no = 0;                 // col2
        int coin = 0;                      // col3
        int coin_check = 0;                // col4
        db->get_int(0, id);
        db->get_int(1, srv);
        db->get_int(2, charac_no);
        db->get_int(3, coin);
        db->get_int(4, coin_check);

        // [b28d-solo-WongWorkEvents_A] P140：ORIG 无 outCount<MAX_ENTRIES 界检
        // （limit 10 保障），删多余守卫。
        if (coin_check == 0 && srv == serverId)
        {
            entries[outCount].m_id = id;
            entries[outCount].m_itemNo = 1;      // ORIG 常量 item_no
            entries[outCount].m_coin = coin;
            entries[outCount].m_characNo = charac_no;
            ++outCount;
        }
        ++row;
    }

    for (int i = 0; i < outCount; ++i)
    {
        UpdateEventEverlandInfo(entries[i].m_id, entries[i].m_itemNo);
        EventCommon::InsertEventItem2EventTable(entries[i].m_characNo, entries[i].m_itemNo,
                                                entries[i].m_coin, 3);
    }
}

#include "WongWorkEvents_A/CheckAlreadyFCComboEventItem.inc"


#include "WongWorkEvents_A/UpdateFCComboEventInfo.inc"


#include "WongWorkEvents_A/ArrangeFCComboEventItem.inc"
{
    const unsigned int MAX_ENTRIES = 10;
    // [b28d-solo-WongWorkEvents_A] P311 反向：struct 行数组复现 ORIG 每列
    // 两步寻址（`shl $4; lea -0x8(%ebp); lea; sub $K`）。
    struct FCComboEntry
    {
        unsigned int m_id;        // [0] id
        int          m_characNo;  // [1]
        int          m_itemNo;    // [2]
        int          m_itemCnt;   // [3]
    };
    FCComboEntry entries[MAX_ENTRIES];

    MySQL* db = GetDBHandle();
    db->set_query(
        "seLect id,item_server_id,item_charac_no,item_no,item_check,item_cnt from "
        "event_combo_entry where m_id=%s and occ_check!=0 limit %d",
        NumberToString(mid, 0), 10);
    if (!db->exec(true))
    {
        // [b28d-solo-WongWorkEvents_A] P227/P139：ORIG 0x083fb9db 先
        // NumberToString 存 %ebx 跨 ctor 保活 + __PRETTY_FUNCTION__ 同址双用。
        cMyTrace(__PRETTY_FUNCTION__, 0x27a, 5)
            ("%s, exec ERROR m_id=%s", __PRETTY_FUNCTION__, NumberToString(mid, 0));
        return;
    }

    int outCount = 0;
    unsigned int row = 0;
    // [b28d-solo-WongWorkEvents_A] ORIG 0x083fba45 尾测 while + 体首
    // `call fetch; xor $1; test; jne exit`（P225 setcc 形）。
    while (row < (unsigned int)db->get_n_rows())
    {
        if (!db->fetch())
            break;
        unsigned int id = 0;               // col0
        int srv = 0;                    // col1
        int charac_no = 0;                 // col2
        int item_no = 0;                   // col3
        int item_check = 0;                // col4
        int item_cnt = 0;                  // col5
        db->get_int(0, id);
        db->get_int(1, srv);
        db->get_int(2, charac_no);
        db->get_int(3, item_no);
        db->get_int(4, item_check);
        db->get_int(5, item_cnt);

        // [b28d-solo-WongWorkEvents_A] P140：ORIG 无 outCount<MAX_ENTRIES 界检。
        if (item_check == 0 && srv == serverId)
        {
            entries[outCount].m_id = id;
            entries[outCount].m_itemNo = item_no;
            entries[outCount].m_itemCnt = item_cnt;
            entries[outCount].m_characNo = charac_no;
            ++outCount;
        }
        ++row;
    }

    for (int i = 0; i < outCount; ++i)
    {
        UpdateFCComboEventInfo(entries[i].m_id, entries[i].m_itemNo);
        EventCommon::InsertEventItem2EventTable(entries[i].m_characNo, entries[i].m_itemNo,
                                                entries[i].m_itemCnt, 7);
    }
}

#include "WongWorkEvents_A/InsertEventItem2EventTable.inc"


#include "WongWorkEvents_A/LoadCharacEventItem.inc"


// ----------------------------------------------------------------------------
// arad_parsing —— ORIG T 0x08483e98 (size 0x115)
// b28d-60：定义已迁至 CDungeonClear.cpp（ORIG TU = DungeonClear.cpp：
//   orig_tu_names.tsv:429 GI 键名归属 + 0x08483e98 结束 0x08483fad 紧接
//   CDungeonClear::AradfromString T 0x08483fae 的地址相邻证据）。
//   声明仍保留在 WongWorkEvents_A.h（消费方无感）。
// ----------------------------------------------------------------------------

}  // namespace WongWork

// ----------------------------------------------------------------------------
