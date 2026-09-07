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
    return (!db->get_int(0, check) || check == 0) ? 0 : 1;
}

// ----------------------------------------------------------------------------
// EventAlttle::ArrangeAlttleEventItem —— ORIG T 0x083fb595 (size 0x2ed)
// b21 verdict=DIFF（N 残余）：188/172。调用点全等（GetDBHandle/set_query/exec/
//   NumberToString×2/cMyTraceC1+cMyTracecl/get_int×5/get_n_rows/fetch/
//   UpdateEventAlttleInfo/InsertEventItem2EventTable）。删冗余 outCount<MAX 界检
//   （ORIG 无，LIMIT 10 保障安全）；残余 cMyTrace 错误路径 NumberToString 求值序/寄存器
//   (N82)、entries[outCount][k] scale 定址 vs ORIG 逐字段 shl/lea/sub、帧 push%ebx vs 栈槽。
//   语义完成，byte 层 N 封顶。
// 取 occ_check!=0 行（limit 10），对 item_check==0 且 server_id 匹配的行收集，
// 之后逐条 Update + InsertEventItem2EventTable(event_code=6)。
// ----------------------------------------------------------------------------
void EventAlttle::ArrangeAlttleEventItem(unsigned int mid, unsigned int serverId)
{
    const unsigned int MAX_ENTRIES = 10;   // local_24 = 10
    unsigned int entries[MAX_ENTRIES][4];  // {mid, characNo, itemNo, cnt} stride 0x10

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "seLect item_server_id,item_charac_no,item_no,item_check,item_cnt from "
        "event_alttle_info where m_id=%s and occ_check!=0 limit %d",
        NumberToString(mid, 0), 10);
    if (!db->exec(true))
    {
        cMyTrace trace("void WongWork::EventAlttle::ArrangeAlttleEventItem("
                       "memberIdentificationNumber_t, unsigned int)", 0x21b, 5);
        trace("%s, exec ERROR m_id=%s",
              "void WongWork::EventAlttle::ArrangeAlttleEventItem("
              "memberIdentificationNumber_t, unsigned int)", NumberToString(mid, 0));
        return;
    }

    int outCount = 0;                       // -0x18（写满区计数）
    unsigned int row = 0;                   // -0x14（fetch 游标）
    while (row < (unsigned int)db->get_n_rows() && db->fetch())
    {
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
            entries[outCount][0] = mid;
            entries[outCount][1] = (unsigned int)charac_no;
            entries[outCount][2] = item_no;
            entries[outCount][3] = item_cnt;
            ++outCount;
        }
        ++row;
    }

    for (int i = 0; i < outCount; ++i)
    {
        UpdateEventAlttleInfo(entries[i][0], entries[i][2]);
        EventCommon::InsertEventItem2EventTable(entries[i][1], entries[i][2], entries[i][3], 6);
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
    int type = CWebEvent::GetItemType(3, itemNo);
    if (type == 1)
    {
        db->set_query(
            "upDate event_sigong_info set item1_check=unix_timestamp(now()) where id=%s",
            NumberToString(id, 0));
    }
    else if (type == 2)
    {
        db->set_query(
            "upDate event_sigong_info set item2_check=unix_timestamp(now()) where id=%s",
            NumberToString(id, 0));
    }
    db->exec(true);
}

// ----------------------------------------------------------------------------
// EventSigong::CheckAlreadyGetSigongItem —— ORIG T 0x083fa69c (size 0x108)
// ----------------------------------------------------------------------------
int EventSigong::CheckAlreadyGetSigongItem(unsigned int id, int itemNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    int type = CWebEvent::GetItemType(3, itemNo);
    if (type == 1)
    {
        db->set_query("seLect item1_check from event_sigong_info where id=%s", NumberToString(id, 0));
    }
    else if (type == 2)
    {
        db->set_query("seLect item2_check from event_sigong_info where id=%s", NumberToString(id, 0));
    }

    if (!db->exec(true))
        return 0;

    int check = 0;
    return (!db->get_int(0, check) || check == 0) ? 0 : 1;
}

// ----------------------------------------------------------------------------
// EventSigong::ArrangeSigongEventItem —— ORIG T 0x083fa855 (size 0x3a9)
// 双栏结构：row[+8]=itemN_no、row[+6]=serverN、row[+2]=checkN；
// itemN 数量为常量：item1=1、item2=200（ORIG movl $0x1,-0x5c/$0xc8,-0x58，
// SQL 未取 cnt 列）。event_code=3。
// ----------------------------------------------------------------------------
void EventSigong::ArrangeSigongEventItem(unsigned int mid, unsigned int serverId)
{
    const unsigned int MAX_ENTRIES = 10;
    unsigned int entries[MAX_ENTRIES][4];  // {id, characNo, itemNo, cnt}

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "seLect id,item1_no,item1_server_id,item1_charac_no,item1_check,"
        "item2_no,item2_server_id,item2_charac_no,item2_check from "
        "event_sigong_info where m_id=%s and occ_check!=0 limit %d",
        NumberToString(mid, 0), 10);
    if (!db->exec(true))
    {
        cMyTrace trace("void WongWork::EventSigong::ArrangeSigongEventItem("
                       "memberIdentificationNumber_t, unsigned int)", 0x123, 5);
        trace("%s, exec ERROR m_id=%s",
              "void WongWork::EventSigong::ArrangeSigongEventItem("
              "memberIdentificationNumber_t, unsigned int)", NumberToString(mid, 0));
        return;
    }

    int outCount = 0;
    unsigned int row = 0;
    while (row < (unsigned int)db->get_n_rows() && db->fetch())
    {
        unsigned int rowBuf[11];           // local_60[0..10]
        rowBuf[2] = 0;                     // item1_check
        rowBuf[3] = 0;                     // item2_check
        rowBuf[4] = 0;                     // item1_server_id
        rowBuf[5] = 0;                     // item2_server_id
        rowBuf[6] = 0;                     // item1_charac_no
        rowBuf[7] = 0;                     // item2_charac_no
        rowBuf[8] = 0;                     // item1_no
        rowBuf[9] = 0;                     // item2_no
        rowBuf[10] = 0;                    // id
        rowBuf[0] = 1;                     // ORIG: item1 常量数量
        rowBuf[1] = 200;                   // ORIG: item2 常量数量

        db->get_int(0, rowBuf[10]);        // id（ORIG 选 unsigned 重载 get_int(int,unsigned&)）
        db->get_int(1, (int&)rowBuf[8]);   // item1_no
        db->get_int(2, (int&)rowBuf[6]);   // item1_server_id
        db->get_int(3, (int&)rowBuf[4]);   // item1_charac_no
        db->get_int(4, (int&)rowBuf[2]);   // item1_check
        db->get_int(5, (int&)rowBuf[9]);   // item2_no
        db->get_int(6, (int&)rowBuf[7]);   // item2_server_id
        db->get_int(7, (int&)rowBuf[5]);   // item2_charac_no
        db->get_int(8, (int&)rowBuf[3]);   // item2_check

        for (int i = 0; i <= 1; ++i)       // ORIG cmpl $0x1 setle
        {
            if (rowBuf[i + 2] == 0 && rowBuf[i + 6] == serverId &&
                outCount < MAX_ENTRIES)
            {
                entries[outCount][0] = rowBuf[10];     // id
                entries[outCount][1] = rowBuf[i + 4];  // itemN_charac_no
                entries[outCount][2] = rowBuf[i + 8];  // itemN_no
                entries[outCount][3] = rowBuf[i];      // 常量数量 1/200
                ++outCount;
            }
        }
        ++row;
    }

    for (int k = 0; k < outCount; ++k)
    {
        UpdateEventSigongInfo(entries[k][0], entries[k][2]);
        EventCommon::InsertEventItem2EventTable(entries[k][1], entries[k][2], entries[k][3], 3);
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
// ----------------------------------------------------------------------------
int EventEverland::CheckAlreadyGetEverlandItem(unsigned int id, int itemNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    if (CWebEvent::GetItemType(4, itemNo) == 1)
    {
        db->set_query("seLect coin_check from event_everland_info where id=%s", NumberToString(id, 0));
    }

    if (!db->exec(true))
        return 0;

    int check = 0;
    return (!db->get_int(0, check) || check == 0) ? 0 : 1;
}

// ----------------------------------------------------------------------------
// EventEverland::ArrangeEverlandEventItem —— ORIG T 0x083fad60 (size 0x32e)
// SQL 无 cnt 列；item_no 恒为常量 1（ORIG movl $0x1,(%eax)）。event_code=3。
// ----------------------------------------------------------------------------
void EventEverland::ArrangeEverlandEventItem(unsigned int mid, unsigned int serverId)
{
    const unsigned int MAX_ENTRIES = 10;
    unsigned int entries[MAX_ENTRIES][4];  // {id, characNo, itemNo=1, coin}

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "seLect id,server_id,charac_no,coin,coin_check from event_everland_info "
        "where m_id=%s and occ_check!=0 limit %d",
        NumberToString(mid, 0), 10);
    if (!db->exec(true))
    {
        cMyTrace trace("void WongWork::EventEverland::ArrangeEverlandEventItem("
                       "memberIdentificationNumber_t, unsigned int)", 0x182, 5);
        trace("%s, exec ERROR m_id=%s",
              "void WongWork::EventEverland::ArrangeEverlandEventItem("
              "memberIdentificationNumber_t, unsigned int)", NumberToString(mid, 0));
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

        if (coin_check == 0 && srv == serverId && outCount < MAX_ENTRIES)
        {
            entries[outCount][0] = id;
            entries[outCount][1] = (unsigned int)charac_no;
            entries[outCount][2] = 1;      // ORIG 常量 item_no
            entries[outCount][3] = coin;
            ++outCount;
        }
        ++row;
    }

    for (int i = 0; i < outCount; ++i)
    {
        UpdateEventEverlandInfo(entries[i][0], entries[i][2]);
        EventCommon::InsertEventItem2EventTable(entries[i][1], entries[i][2], entries[i][3], 3);
    }
}

// ----------------------------------------------------------------------------
// EventFCCombo::CheckAlreadyFCComboEventItem —— ORIG T 0x083fb883 (size 0xa5)
// 使用自有 GetDBHandle()（同型 GlobalData::s_db_mgr, 1, 0）。
// ----------------------------------------------------------------------------
int EventFCCombo::CheckAlreadyFCComboEventItem(unsigned int id, int itemNo)
{
    MySQL* db = GetDBHandle();
    db->set_query("seLect item_check from event_combo_entry where id=%s", NumberToString(id, 0));
    if (!db->exec(true))
        return 0;

    int check = 0;
    return (!db->get_int(0, check) || check == 0) ? 0 : 1;
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
// ----------------------------------------------------------------------------
void EventFCCombo::ArrangeFCComboEventItem(unsigned int mid, unsigned int serverId)
{
    const unsigned int MAX_ENTRIES = 10;
    unsigned int entries[MAX_ENTRIES][4];

    MySQL* db = GetDBHandle();
    db->set_query(
        "seLect id,item_server_id,item_charac_no,item_no,item_check,item_cnt from "
        "event_combo_entry where m_id=%s and occ_check!=0 limit %d",
        NumberToString(mid, 0), 10);
    if (!db->exec(true))
    {
        cMyTrace trace("void WongWork::EventFCCombo::ArrangeFCComboEventItem("
                       "memberIdentificationNumber_t, unsigned int)", 0x27a, 5);
        trace("%s, exec ERROR m_id=%s",
              "void WongWork::EventFCCombo::ArrangeFCComboEventItem("
              "memberIdentificationNumber_t, unsigned int)", NumberToString(mid, 0));
        return;
    }

    int outCount = 0;
    unsigned int row = 0;
    while (row < (unsigned int)db->get_n_rows() && db->fetch())
    {
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

        if (item_check == 0 && srv == serverId && outCount < MAX_ENTRIES)
        {
            entries[outCount][0] = id;
            entries[outCount][1] = (unsigned int)charac_no;
            entries[outCount][2] = item_no;
            entries[outCount][3] = item_cnt;
            ++outCount;
        }
        ++row;
    }

    for (int i = 0; i < outCount; ++i)
    {
        UpdateFCComboEventInfo(entries[i][0], entries[i][2]);
        EventCommon::InsertEventItem2EventTable(entries[i][1], entries[i][2], entries[i][3], 7);
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
        cMyTrace trace("void WongWork::EventCommon::LoadCharacEventItem("
                       "memberIdentificationNumber_t, unsigned int, "
                       "memberIdentificationNumber_t)", 0x37, 5);
        trace("DB_LoadInventory::GetUserInven, exec ERROR charac_no=%u", characNo);
        return;
    }

    // ORIG 0x083f9d60：if 条件内直接调用 get_n_rows()（第一次）；
    // 下方 int rowCount = get_n_rows() 为第二次调用（0x083f9e09）。忠实保留。
    if (db->get_n_rows() == 0)
        return;

    Stream* stream = GlobalData::s_stream_pool->Acquire("DBEventWork.inl", 0x3e);
    CStreamGuard guard(stream, true);
    (*guard.operator*()) << 0x4b;                      // msg id（operator<< int @0x80c8c56）
    (*guard.operator*()) << -1;                        // size 占位

    SigLoadEventItemView* sig =
        reinterpret_cast<SigLoadEventItemView*>(
            guard.operator->()->GetInBuffer<SIG_LOAD_EVENTITEM>());
    sig->m_eventId = eventId;
    sig->m_characNo = characNo;

    int rowCount = db->get_n_rows();                   // 第二次调用（ORIG 0x083f9e09）
    int loaded = 0;                                    // fetch 游标
    while (loaded < rowCount && db->fetch())
    {
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
// ORIG 的 stHackInfo_t / HackTypeInfo_t 方法均为 inline（W 弱符号），其产生点
// 在原 TU 中由实际调用触发；本批头内实现为忠实 inline，此处补 odr-use 以在本
// TU 产出对应的 W 符号（不改变任何语义/布局）。
// ----------------------------------------------------------------------------
namespace
{
void EmitHackPodWeakSymbols()
{
    WongWork::HackTypeInfo_t infoDefault;   // C1Ev/C2Ev
    WongWork::HackTypeInfo_t infoFull((WongWork::ENUM_HACKTYPE)1, 0,
                                      (WongWork::ENUM_HACKTYPE)2, 0, 0, 0, 0);
                                            // C1(E...)C2
    infoFull.CopyStruct(infoDefault);       // CopyStruct
    (void)infoFull;

    WongWork::stHackInfo_t hackInfo;        // D1（作用域结束 complete dtor）
    hackInfo.~stHackInfo_t();               // D2（base dtor 显式调用）
}
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
