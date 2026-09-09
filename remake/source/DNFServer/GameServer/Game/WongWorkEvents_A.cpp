// ============================================================================
// WongWorkEvents_A.cpp —— WongWork 事件域 ORIG 符号转写（G2 批，独占 TU）
// 依据 docs/class_func_reports/WongWork__Event{Alttle,Sigong,Everland,FCCombo,Common}/
// 与 objdump df_game_r（HackTypeInfo_t/arad_parsing 直反汇编）。
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

// ----------------------------------------------------------------------------
// EventFCCombo::GetDBHandle —— ORIG W 0x0844dc7c：
//   return DBMgr::GetDBHandle(GlobalData::s_db_mgr, 1, 0);
// ----------------------------------------------------------------------------
MySQL* EventFCCombo::GetDBHandle()
{
    return GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                             (ENUM_SERVER_GROUP)0);
}

// ----------------------------------------------------------------------------
// EventAlttle::UpdateEventAlttleInfo —— ORIG T 0x083fb530 (size 0x64)
//   UPDATE item_check=unix_timestamp(now())
// ----------------------------------------------------------------------------
void EventAlttle::UpdateEventAlttleInfo(unsigned int uid, int itemNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "upDate event_alttle_info set item_check=unix_timestamp(now()) where m_id=%s",
        NumberToString(uid, 0));
    db->exec(true);
}

// ----------------------------------------------------------------------------
// EventAlttle::CheckAlreadyGetAlttleItem —— ORIG T 0x083fb472 (size 0xbd)
// exec 成功且 get_int(item_check)!=0 → 已领(1)；否则 0。
// ----------------------------------------------------------------------------
int EventAlttle::CheckAlreadyGetAlttleItem(unsigned int uid, int itemNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect item_check from event_alttle_info where m_id=%s", NumberToString(uid, 0));
    if (!db->exec(true))
        return 0;

    int check = 0;
    // ORIG 尾段 if 块常量岛（非三目 sete 折叠）：if/return 分离形。
    if (!db->get_int(0, check) || check == 0)
        return 0;
    return 1;
}

// ----------------------------------------------------------------------------
// EventAlttle::ArrangeAlttleEventItem —— ORIG T 0x083fb595 (size 0x2ed)
// [b28d-solo-WongWorkEvents_A] AE（推翻 b21「N 封顶」判词，188=188）：
// ① P227/P139 错误路径改全表达式临时链 + __PRETTY_FUNCTION__（恢复
//    NumberToString→%ebx 跨 ctor 保活 + push %ebx/sub $0xf4）；
// ② P311 反向 struct 行数组（每列两步 `shl $4; lea -0x8(%ebp); lea; sub $K`）；
// ③ P171/P232 补循环体内 dead 局部 `unsigned int unused = 0;`（ORIG
//    `movl $0,-0x10(%ebp)`，把 db/outCount/row/列局部整体下移 4 槽）；
// ④ P225 尾测 while + `if (!fetch()) break;`。
// 取 occ_check!=0 行（limit 10），对 item_check==0 且 server_id 匹配的行收集，
// 之后逐条 Update + InsertEventItem2EventTable(event_code=6)。
// ----------------------------------------------------------------------------
void EventAlttle::ArrangeAlttleEventItem(unsigned int mid, unsigned int serverId)
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

// ----------------------------------------------------------------------------
// EventSigong::UpdateEventSigongInfo —— ORIG T 0x083fa7a5 (size 0xe0)
// 按 CWebEvent::GetItemType(3,itemNo)==1/2 选 item1_check/item2_check 栏。
// ----------------------------------------------------------------------------
void EventSigong::UpdateEventSigongInfo(unsigned int id, int itemNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    // P17：ORIG 单读双判线性 cmp/je 链（body 后置）= 两 case switch 形。
    switch (CWebEvent::GetItemType(3, itemNo))
    {
        case 1:
            db->set_query(
                "upDate event_sigong_info set item1_check=unix_timestamp(now()) where id=%s",
                NumberToString(id, 0));
            break;
        case 2:
            db->set_query(
                "upDate event_sigong_info set item2_check=unix_timestamp(now()) where id=%s",
                NumberToString(id, 0));
            break;
    }
    db->exec(true);
}

// ----------------------------------------------------------------------------
// EventSigong::CheckAlreadyGetSigongItem —— ORIG T 0x083fa69c (size 0x108)
// [b28d-solo-WongWorkEvents_A] AE + 真语义 bug 修正：尾段极性原为
// `(get_int() && check!=0) ? 0 : 1`（旧写反，get_int 失败误返 1）；修法 =
// P251/P279 switch 直传（GetItemType）+ P226 语句表达式（尾段）。
// ----------------------------------------------------------------------------
int EventSigong::CheckAlreadyGetSigongItem(unsigned int id, int itemNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    // [b28d-solo-WongWorkEvents_A] P251/P279：ORIG 0x083fa6d5 无槽 cmp 链
    // （`call; cmp $1,%eax; je; cmp $2,%eax; je; jmp`）⇒ 原码是
    // `switch (GetItemType(...))` 直传，不是具名 int + if/else 链
    // （具名局部令 4.4 产 `mov %eax,-N; cmpl $K,-N` 且整帧槽号漂移）。
    switch (CWebEvent::GetItemType(3, itemNo))
    {
        case 1:
            db->set_query("seLect item1_check from event_sigong_info where id=%s",
                          NumberToString(id, 0));
            break;
        case 2:
            db->set_query("seLect item2_check from event_sigong_info where id=%s",
                          NumberToString(id, 0));
            break;
    }

    if (!db->exec(true))
        return 0;

    int check = 0;
    // [b28d-solo-WongWorkEvents_A] 真语义 bug 修正 + P226 条件向：
    // ORIG 0x083fa779 `xor $1;test;jne L1`（get_int 失败→L1）+
    // `test check;jne L2`（check!=0→L2）+ `L1:mov $1` / `L2:mov $0` +
    // `test %al;je L4` + `mov $0` / `L4:mov $1` ⇒ 语义 =
    // (get_int() && check!=0) ? 1 : 0；旧写 `if(!get_int) return 1;` 极性反
    // （get_int 失败误返 1）。裸语句表达式复现 4.1 短路物化岛（4.4 对裸
    // `!call()||…` 折直 test/je 且省尾岛）。
    if (({ !db->get_int(0, check) || check == 0; }))
        return 0;
    return 1;
}

// ----------------------------------------------------------------------------
// EventSigong::ArrangeSigongEventItem —— ORIG T 0x083fa855 (size 0x3a9)
// 双栏结构：row[+8]=itemN_no、row[+6]=serverN、row[+2]=checkN；
// itemN 数量为常量：item1=1、item2=200（ORIG movl $0x1,-0x5c/$0xc8,-0x58，
// SQL 未取 cnt 列）。event_code=3。
// [b28d-solo-WongWorkEvents_A] AE：P227/P139 错误路径全表达式临时链 +
// __PRETTY_FUNCTION__；P140 删多余守卫；P225 尾测 while/break；P311 反向
// 「每栏独立 2 元素数组 + 独立 id」复现 disp 折叠（局部 struct 视图无效）。
// ----------------------------------------------------------------------------
void EventSigong::ArrangeSigongEventItem(unsigned int mid, unsigned int serverId)
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

// ----------------------------------------------------------------------------
// EventEverland::UpdateEventEverlandInfo —— ORIG T 0x083face3 (size 0xd4)
// GetItemType(4,itemNo)==1 时更新 coin_check。
// ----------------------------------------------------------------------------
void EventEverland::UpdateEventEverlandInfo(unsigned int id, int itemNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    // ORIG T 0x083face3：GetItemType 结果 `cmp $0x1,%eax; jne`（直比无 bool
    // 物化）。4.4 对 `if (f()==1)` 恒产 `cmp $1; sete %al; test; je`，switch
    // case 1 直比才复现 jne（P17：jump-table/线性 cmp 定 switch，本例单 case
    // 即 cmp $1/jne 直落）。
    switch (CWebEvent::GetItemType(4, itemNo))
    {
    case 1:
        db->set_query(
            "upDate event_everland_info set coin_check=unix_timestamp(now()) where id=%s",
            NumberToString(id, 0));
        break;
    default:
        break;
    }
    db->exec(true);
}

// ----------------------------------------------------------------------------
// EventEverland::CheckAlreadyGetEverlandItem —— ORIG T 0x083fac0d (size 0xd5)
// [b28d-solo-WongWorkEvents_A] AE：P286 单 case switch（GetItemType 直比
// `cmp $1,%eax; jne`）+ P226 语句表达式（尾段短路物化岛）。
// ----------------------------------------------------------------------------
int EventEverland::CheckAlreadyGetEverlandItem(unsigned int id, int itemNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    // [b28d-solo-WongWorkEvents_A] P286：ORIG 0x083fac46 `cmp $1,%eax; jne`
    // 直比无 bool 物化；4.4 对 `if (f()==1)` 恒 `cmp $1; sete %al; test; je`
    // （+2 条）。单 case switch 直比即复现 ORIG（同 UpdateEventEverlandInfo）。
    switch (CWebEvent::GetItemType(4, itemNo))
    {
    case 1:
        db->set_query("seLect coin_check from event_everland_info where id=%s", NumberToString(id, 0));
        break;
    default:
        break;
    }

    if (!db->exec(true))
        return 0;

    int check = 0;
    // [b28d-solo-WongWorkEvents_A] P226 条件向：ORIG 0x083facb7 `xor $1;
    // test;jne L1` + `test check;jne L2` + `L1:mov $1` / `L2:mov $0` +
    // `test %al;je L4` + `mov $0` / `L4:mov $1` 为 4.1 短路 bool 物化岛；
    // 4.4 对裸 `!call()||…` 折直 test/je 并省尾岛。裸语句表达式（无具名
    // 变量）令 4.4 保留物化岛且零槽（P226 条件向，FCCombo/Sigong 同形）。
    if (({ !db->get_int(0, check) || check == 0; }))
        return 0;
    return 1;
}

// ----------------------------------------------------------------------------
// EventEverland::ArrangeEverlandEventItem —— ORIG T 0x083fad60 (size 0x32e)
// SQL 无 cnt 列；item_no 恒为常量 1（ORIG movl $0x1,(%eax)）。event_code=3。
// [b28d-solo-WongWorkEvents_A] AE：P227/P139 错误路径全表达式临时链 +
// __PRETTY_FUNCTION__；P140 删 outCount<MAX 多余守卫；P311 反向 struct 行
// 数组复现两步寻址；P225 尾测 while/break。
// ----------------------------------------------------------------------------
void EventEverland::ArrangeEverlandEventItem(unsigned int mid, unsigned int serverId)
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

// ----------------------------------------------------------------------------
// EventFCCombo::CheckAlreadyFCComboEventItem —— ORIG T 0x083fb883 (size 0xa5)
// 使用自有 GetDBHandle()（同型 GlobalData::s_db_mgr, 1, 0）。
// [b28d-solo-WongWorkEvents_A] AE：P226 条件向裸语句表达式复现 4.1 短路
// bool 物化岛（`xor $1;test;jne` + 常量岛 + 尾 `test;je`）。
// ----------------------------------------------------------------------------
int EventFCCombo::CheckAlreadyFCComboEventItem(unsigned int id, int itemNo)
{
    MySQL* db = GetDBHandle();
    db->set_query("seLect item_check from event_combo_entry where id=%s", NumberToString(id, 0));
    if (!db->exec(true))
        return 0;

    int check = 0;
    // [b28d-solo-WongWorkEvents_A] P226 条件向：ORIG 0x083fb8fd 起
    // `xor $1;test %al;jne L1` + `test check;jne L2` + `L1:mov $1;jmp L3` +
    // `L2:mov $0` + `L3:test %al;je L4` + `mov $0;jmp` + `L4:mov $1` 为
    // 4.1 短路 bool 物化岛；4.4 对裸 `!call()||…` 折直 test/je。裸语句
    // 表达式（无具名变量）令 4.4 保留该物化岛且零槽。
    if (({ !db->get_int(0, check) || check == 0; }))
        return 0;
    return 1;
}

// ----------------------------------------------------------------------------
// EventFCCombo::UpdateFCComboEventInfo —— ORIG T 0x083fb929 (size 0x4d)
// ----------------------------------------------------------------------------
void EventFCCombo::UpdateFCComboEventInfo(unsigned int id, int itemNo)
{
    MySQL* db = GetDBHandle();
    db->set_query("upDate event_combo_entry set item_check=unix_timestamp(now()) where id=%s",
                  NumberToString(id, 0));
    db->exec(true);
}

// ----------------------------------------------------------------------------
// EventFCCombo::ArrangeFCComboEventItem —— ORIG T 0x083fb976 (size 0x2ef)
// 6 列查询含真实 item_cnt。event_code=7。
// [b28d-solo-WongWorkEvents_A] AE：P227/P139 错误路径全表达式临时链 +
// __PRETTY_FUNCTION__；P140 删多余守卫；P311 反向 struct 行数组；P225 尾测
// while/break。
// ----------------------------------------------------------------------------
void EventFCCombo::ArrangeFCComboEventItem(unsigned int mid, unsigned int serverId)
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

// ----------------------------------------------------------------------------
// EventCommon::InsertEventItem2EventTable —— ORIG T 0x083f9b54 (size 0x127)
// db handle idx=3；成功记录 trace 并返回 @@identity，失败返回 0。
// ----------------------------------------------------------------------------
unsigned int EventCommon::InsertEventItem2EventTable(unsigned int characNo,
                                                     unsigned int itemId,
                                                     unsigned int stackCount,
                                                     unsigned int eventCode)
{
    static const char FUNC_NAME[] =
        "unsigned int WongWork::EventCommon::InsertEventItem2EventTable("
        "unsigned int, unsigned int, unsigned int, unsigned int)";
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "inSert into charac_event_items(charac_no,it_id,stack_count,event_code,"
        "reg_time) values(%d,%d,%d,%d,now())",
        characNo, itemId, stackCount, eventCode);
    if (!db->exec(true))
    {
        cMyTrace fail(FUNC_NAME, 0x25, 0);
        fail("FAIL : insert into charac_event_items(charac_no,it_id,stack_count,"
             "event_code,reg_time) values(%d,%d,%d,%d,now())",
             characNo, itemId, stackCount, eventCode);
        return 0;
    }

    cMyTrace ok(FUNC_NAME, 0x28, 0);
    ok("SUCCESS : insert into charac_event_items(charac_no,it_id,stack_count,"
       "event_code,reg_time) values(%d,%d,%d,%d,now())",
       characNo, itemId, stackCount, eventCode);
    return DBCommon::GetIdentity(db);
}

// ----------------------------------------------------------------------------
// EventCommon::LoadCharacEventItem —— ORIG T 0x083f9c7b (size 0x2b2)
// loadId==0 按角色取 5 条 delete_flag=0；否则按行 id 单取。
// 组包 header 0x4b + size(-1)，SIG_LOAD_EVENTITEM 装载后投递 msgq queue 1。
// [b28d-solo-WongWorkEvents_A] AE：P259 Acquire 内联实参（去 Stream* 具名
// 局部槽）+ P225 尾测 while/break + P221 cMyTrace 全表达式临时链（翻互斥块
// cMyTrace/guard 槽方向）。
// ----------------------------------------------------------------------------
void EventCommon::LoadCharacEventItem(unsigned int eventId,
                                      unsigned int characNo,
                                      unsigned int loadId)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                  (ENUM_SERVER_GROUP)0);
    if (loadId == 0)
    {
        db->set_query(
            "seLect id,it_id,stack_count,event_code from charac_event_items "
            "where charac_no=%d and delete_flag=0 limit %d", characNo, 5);
    }
    else
    {
        db->set_query(
            "seLect id,it_id,stack_count,event_code from charac_event_items "
            "where id=%s and charac_no=%d", NumberToString(loadId, 0), characNo);
    }

    if (!db->exec(true))
    {
        // [b28d-solo-WongWorkEvents_A] P221：ORIG cMyTrace 槽 -0x28 浅于
        // CStreamGuard 槽 -0x30；4.4 具名声明序恒反向（cMyTrace -0x30 /
        // guard -0x20）⇒ 改全表达式临时链翻互斥块槽方向。
        cMyTrace("void WongWork::EventCommon::LoadCharacEventItem("
                 "memberIdentificationNumber_t, unsigned int, "
                 "memberIdentificationNumber_t)", 0x37, 5)
            ("DB_LoadInventory::GetUserInven, exec ERROR charac_no=%u", characNo);
        return;
    }

    // ORIG 0x083f9d60：if 条件内直接调用 get_n_rows()（第一次）；
    // 下方 int rowCount = get_n_rows() 为第二次调用（0x083f9e09）。忠实保留。
    if (db->get_n_rows() == 0)
        return;

    // [b28d-solo-WongWorkEvents_A] P259：ORIG 0x083f9d8f `mov %eax,0x4(%esp)`
    // 直传 Acquire 结果，无 Stream* 具名局部（具名槽令 db/cMyTrace/guard 整体
    // 下移 4）。内联实参复现。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBEventWork.inl", 0x3e), true);
    (*guard.operator*()) << 0x4b;                      // msg id（operator<< int @0x80c8c56）
    (*guard.operator*()) << -1;                        // size 占位

    SigLoadEventItemView* sig =
        reinterpret_cast<SigLoadEventItemView*>(
            guard.operator->()->GetInBuffer<SIG_LOAD_EVENTITEM>());
    sig->m_eventId = eventId;
    sig->m_characNo = characNo;

    int rowCount = db->get_n_rows();                   // 第二次调用（ORIG 0x083f9e09）
    int loaded = 0;                                    // fetch 游标
    // [b28d-solo-WongWorkEvents_A] ORIG 0x083f9e18 `movl $0,-0xc; jmp cond` +
    // body 首 `call fetch; xor $1; test; jne exit` + cond
    // `mov loaded; cmp rowCount; setl; test; jne body`（P225 setcc 形）⇒
    // 尾测 while + `if (!fetch()) break;`，非 `loaded<n && fetch()` 短路
    // （后者把 fetch 放进条件块且用 jge 直跳）。
    while (loaded < rowCount)
    {
        if (!db->fetch())
            break;
        db->get_uint(0, sig->m_entries[loaded].m_dbId);
        db->get_uint(1, sig->m_entries[loaded].m_itemId);
        db->get_uint(2, sig->m_entries[loaded].m_stackCount);
        db->get_uint(3, sig->m_entries[loaded].m_eventCode);
        ++loaded;
    }
    // +0x08 = 第二次 get_n_rows()=rowCount（ORIG `mov -0x14,%edx; mov %edx,0x8`
    // 直写本地行数，非 fetch 已装载数 loaded ── fetch 提前失败时仍写全行数，
    // 与调用方消费口径一致；勿臆改）。
    sig->m_itemCount = rowCount;

    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
}

// ----------------------------------------------------------------------------
// arad_parsing —— ORIG T 0x08483e98 (size 0x115)
// start=find_first_not_of(delims,pos)/end=find_first_of(...)，非全 npos 则循环
// substr 入 vector（move push_back）；尾段 end==npos 但 start!=npos 也输出。
// ----------------------------------------------------------------------------
void arad_parsing(const std::string& src,
                  std::vector<std::string>& out,
                  const std::string& delims)
{
    std::string::size_type start = src.find_first_not_of(delims, 0);
    std::string::size_type end = src.find_first_of(delims, start);
    out.clear();

    while (end != std::string::npos || start != std::string::npos)
    {
        out.push_back(src.substr(start, end - start));
        start = src.find_first_not_of(delims, end);
        end = src.find_first_of(delims, start);
    }
}

}  // namespace WongWork

// ----------------------------------------------------------------------------
