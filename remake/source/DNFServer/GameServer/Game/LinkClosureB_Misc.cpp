// ============================================================================
// LinkClosureB_Misc.cpp — /tmp/undef_bizB.txt 杂项域收口（LinkClosureB 批 B）
//
// 逐符号 ORIG nm/objdump/报告核实后忠实转写；唯一 owner = 本 TU：
//   WongWork::DBCommon::GetIdentity(MySQL*)              T 0x083f9ad4
//   WongWork::CWebEvent::GetItemType(int,int)            T 0x086c1aba
//   WongWork::EventATI::{CheckAlreadyGetATIItem(ji)      T 0x083f9f2e
//                        UpdateEventATIInfo(ji)          T 0x083fa0ad
//                        ArrangeATIEventItem(jj)}        T 0x083fa1d3
//   WongWork::EventMuz::{CheckAlreadyGetMuzItem(ji)      T 0x083fb047
//                        UpdateEventMuzInfo(ji)          T 0x083fb105
//                        ArrangeMuzEventItem(jj)}        T 0x083fb16a
//   AvatarCoin::HistoryLog::SubLog(CUser*,unsigned int)  T 0x0817ff54
//   TOD_UserState::getLastEnterLayer() const             T 0x08643942
//   TimerStartMatching::registNextTimer(long,int,int)    T 0x08637e14
//   advanceAltarstatistics_（ORIG B @0x09400300 全局实例）B
//   CAutoMarketConditionsControl::{GetItemDropRate(i)    T 0x082f8ae8
//                                 ChangeItemRarity(PiS0id)} T 0x082f8c8c
//
// GetInvenTypeFromItemSpace 已由 CTitleBook.cpp 提供（核对同跳表）；roundFloat /
// Drop 族 / ARAD server 族 / CompoundAvatar 族等由本批兄弟 TU 承载。
// ============================================================================

#include <stdio.h>
#include <math.h>

#include "MySQL.h"
#include "DBMgr.h"
#include "GlobalData.h"
#include "GameTypes.h"        // cMyTrace
#include "CInventory.h"       // NumberToString
#include "WongWorkEvents_A.h" // EventCommon::InsertEventItem2EventTable
#include "CUser.h"
#include "AvatarCoin.h"
#include "DataManagerInitDepends.h" // TOD_UserState
#include "TimerQueue.h"
#include "TimerDispatcher.h"  // TimerStartMatching
#include "CAdvanceAltarLog.h" // advanceAltarstatistics_
#include "advancealtar.h"    // advancealtar::HistoryLog（advanceAltarstatistics_ 类静态成员声明）

// ---------------------------------------------------------------------------
// WongWork::DBCommon::GetIdentity —— ORIG T 0x083f9ad4（报告 DBCommon/GetIdentity.md）
// seLect @@identity → exec(true)/fetch/get_uint(0)；任一步失败返回 0。
// ---------------------------------------------------------------------------
namespace WongWork {

class DBCommon
{
public:
    static unsigned int GetIdentity(MySQL* db);  // ORIG T 0x083f9ad4
};

unsigned int DBCommon::GetIdentity(MySQL* db)
{
    unsigned int identity;  // ORIG 无预清 store（仅 get_uint 出参写回）
    db->set_query("seLect @@identity");
    if (!db->exec(true))
        return 0;
    if (!db->fetch())
        return 0;
    if (!db->get_uint(0, identity))
        return 0;
    return identity;
}

}  // namespace WongWork

// ---------------------------------------------------------------------------
// WongWork::CWebEvent::GetItemType —— ORIG T 0x086c1aba（报告+objdump 核对）
// kind==2 离散表分派；kind==3 {8→2, 1→1}；其余 0。
// ---------------------------------------------------------------------------
namespace WongWork {

// CWebEvent 权威声明已收敛至 GlobalData.h:274(Main 沉降仲裁,含 GetItemType);
// 本 TU 的局部 class 视图已删除,仅保留 ORIG T 定义。

// [b28d-D9] N82/P46：switch 决策树+叶块序+帧/槽号差；调用点差集待核（ORIG 0x086c1aba sz 0x000000ed）
int CWebEvent::GetItemType(int eventKind, int itemNo)
{
    int type = 0;
    if (eventKind == 2)
    {
        switch (itemNo)
        {
        case 0xae68: type = 1; break;
        case 0x659d: type = 3; break;
        case 999:    type = 4; break;   // 0x3e7
        case 0x9ed0: type = 1; break;
        case 0xa060: type = 2; break;
        case 0xbf9c: type = 2; break;
        case 0xaff8: type = 2; break;
        case 0xbe0c: type = 1; break;
        case 0xcd81: type = 1; break;
        case 0xcf11: type = 2; break;
        default:     type = 0; break;
        }
    }
    else if (eventKind == 3)
    {
        if (itemNo == 8)
            type = 2;
        else if (itemNo == 0x659e)
            type = 1;
    }
    if (eventKind == 3 && itemNo == 1)
        type = 1;
    return type;
}

}  // namespace WongWork

// ---------------------------------------------------------------------------
// WongWork::EventATI —— ORIG 事件族（报告 WongWork__EventATI/*）。
// 类与 DB_Login.cpp 的调用视图一致（全静态成员，无数据成员）。
// ---------------------------------------------------------------------------
namespace WongWork {

struct EventATI
{
    static int CheckAlreadyGetATIItem(unsigned int uid, int itemNo);
    static void UpdateEventATIInfo(unsigned int uid, int itemNo);
    static void ArrangeATIEventItem(unsigned int mid, unsigned int serverId);
};

// CheckAlreadyGetATIItem —— ORIG T 0x083f9f2e (size 0xd9)
// [b28d-D9] 修复：P46 叶块序：case1 写于 case2 前（交换后 .o AE 96/96）（ORIG 0x083f9f2e sz 0x0000017f）
int EventATI::CheckAlreadyGetATIItem(unsigned int uid, int itemNo)
{
    // ORIG 直调 DBMgr::GetDBHandle(1,0)（禁 helper 包装遮蔽该 call，P167 判据）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    switch (CWebEvent::GetItemType(2, itemNo))
    {
    case 1:
        db->set_query("seLect avata_check1 from event_ati_info where id=%s",
                      NumberToString(uid, 0));
        break;
    case 2:
        db->set_query("seLect avata_check2 from event_ati_info where id=%s",
                      NumberToString(uid, 0));
        break;
    case 3:
        db->set_query("seLect item_check from event_ati_info where id=%s",
                      NumberToString(uid, 0));
        break;
    case 4:
        db->set_query("seLect cera_check from event_ati_info where id=%s",
                      NumberToString(uid, 0));
        break;
    default:
        break;  // ORIG default 落入 exec 段（Ghidra C 证实无 return 0）
    }
    if (!db->exec(true))
        return 0;
    int check = 0;
    if (!db->get_int(0, check) || check == 0)
        return 0;
    return 1;
}

// UpdateEventATIInfo —— ORIG T 0x083fa0ad (size 0x126)
// [b28d-D9] 修复：P46 叶块序：case1 写于 case2 前（交换后 .o AE 70/70）（ORIG 0x083fa0ad sz 0x00000126）
void EventATI::UpdateEventATIInfo(unsigned int uid, int itemNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    switch (CWebEvent::GetItemType(2, itemNo))
    {
    case 1:
        db->set_query("upDate event_ati_info set avata_check1=unix_timestamp(now()) where id=%s",
                      NumberToString(uid, 0));
        break;
    case 2:
        db->set_query("upDate event_ati_info set avata_check2=unix_timestamp(now()) where id=%s",
                      NumberToString(uid, 0));
        break;
    case 3:
        db->set_query("upDate event_ati_info set item_check=unix_timestamp(now()) where id=%s",
                      NumberToString(uid, 0));
        break;
    case 4:
        db->set_query("upDate event_ati_info set cera_check=unix_timestamp(now()) where id=%s",
                      NumberToString(uid, 0));
        break;
    default:
        break;
    }
    db->exec(true);
}

// ArrangeATIEventItem —— ORIG T 0x083fa1d3 (size 0x287)
// 行收集：item_check==0 且 item_server_id==serverId 一条；
// avata_server_id==serverId 时 avata_check1==0 / avata_check2==0 各一条。
// [b28d-D9] P75/N80：变参装载序 + 帧差 0x114 vs 0x118（ORIG 0x083fa1d3 sz 0x000004c9）
void EventATI::ArrangeATIEventItem(unsigned int mid, unsigned int serverId)
{
    const char* kFunc =
        "void WongWork::EventATI::ArrangeATIEventItem(memberIdentificationNumber_t, unsigned int)";
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "seLect id,cera,cera_check,item_no,item_server_id,item_charac_no,item_check,"
        "avata_no1,avata_no2,avata_server_id,avata_charac_no,avata_check1,avata_check2 "
        "from event_ati_info where m_id=%s and occ_check!=0 limit %d",
        NumberToString(mid, 0), 10);
    if (!db->exec(true))
    {
        cMyTrace trace(kFunc, 0xa1, 5);
        trace("%s, exec ERROR m_id=%s", kFunc, NumberToString(mid, 0));
        return;
    }

    unsigned int rows[10][4];      // [dbId, stackCount?, itemId, flag]
    int count = 0;
    int scanned = 0;
    while (scanned < db->get_n_rows() && db->fetch())
    {
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

        if (itemCheck == 0 && itemServerId == static_cast<int>(serverId) && count < 10)
        {
            rows[count][0] = static_cast<unsigned int>(rowId);
            rows[count][1] = static_cast<unsigned int>(itemCharacNo);
            rows[count][2] = static_cast<unsigned int>(itemNo);
            rows[count][3] = 1;
            ++count;
        }
        if (avataServerId == static_cast<int>(serverId))
        {
            if (avataCheck1 == 0 && count < 10)
            {
                rows[count][0] = static_cast<unsigned int>(rowId);
                rows[count][1] = static_cast<unsigned int>(avataCharacNo);
                rows[count][2] = static_cast<unsigned int>(avataNo1);
                rows[count][3] = 1;
                ++count;
            }
            if (avataCheck2 == 0 && count < 10)
            {
                rows[count][0] = static_cast<unsigned int>(rowId);
                rows[count][1] = static_cast<unsigned int>(avataCharacNo);
                rows[count][2] = static_cast<unsigned int>(avataNo2);
                rows[count][3] = 1;
                ++count;
            }
        }
        ++scanned;
    }

    for (int i = 0; i < count; ++i)
    {
        UpdateEventATIInfo(rows[i][0], static_cast<int>(rows[i][2]));
        EventCommon::InsertEventItem2EventTable(rows[i][1], rows[i][2],
                                                rows[i][3], 2);
    }
}

}  // namespace WongWork

// ---------------------------------------------------------------------------
// WongWork::EventMuz —— ORIG 事件族（报告 WongWork__EventMuz/*）
// ---------------------------------------------------------------------------
namespace WongWork {

struct EventMuz
{
    static int CheckAlreadyGetMuzItem(unsigned int uid, int itemNo);
    static void UpdateEventMuzInfo(unsigned int uid, int itemNo);
    static void ArrangeMuzEventItem(unsigned int mid, unsigned int serverId);
};

// CheckAlreadyGetMuzItem —— ORIG T 0x083fb047 (size 0xba)
int EventMuz::CheckAlreadyGetMuzItem(unsigned int uid, int itemNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect item_check from event_muz_entry where id=%s",
                  NumberToString(uid, 0));
    if (!db->exec(true))
        return 0;
    int check = 0;
    if (!db->get_int(0, check) || check == 0)
        return 0;
    return 1;
}

// UpdateEventMuzInfo —— ORIG T 0x083fb105 (size 0x63)
void EventMuz::UpdateEventMuzInfo(unsigned int uid, int itemNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("upDate event_muz_entry set item_check=unix_timestamp(now()) where id=%s",
                  NumberToString(uid, 0));
    db->exec(true);
}

// ArrangeMuzEventItem —— ORIG T 0x083fb16a (size 0x240)
// [b28d-D9] P75/N80：变参装载序（$0xa 槽位互换）+ 帧差 0xf4 vs 0xf8（ORIG 0x083fb16a sz 0x00000308）
void EventMuz::ArrangeMuzEventItem(unsigned int mid, unsigned int serverId)
{
    const char* kFunc =
        "void WongWork::EventMuz::ArrangeMuzEventItem(memberIdentificationNumber_t, unsigned int)";
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "seLect id,item_server_id,item_charac_no,item_no,item_check,item_cnt "
        "from event_muz_entry where m_id=%s and occ_check!=0 limit %d",
        NumberToString(mid, 0), 10);
    if (!db->exec(true))
    {
        cMyTrace trace(kFunc, 0x1ce, 5);
        trace("%s, exec ERROR m_id=%s", kFunc, NumberToString(mid, 0));
        return;
    }

    unsigned int rows[10][4];
    int count = 0;
    int scanned = 0;
    while (scanned < db->get_n_rows() && db->fetch())
    {
        unsigned int rowId = 0; int itemServerId = 0; int itemCharacNo = 0;
        int itemNo = 0; int itemCheck = 0; int itemCnt = 0;
        db->get_int(0, rowId);
        db->get_int(1, itemServerId);
        db->get_int(2, itemCharacNo);
        db->get_int(3, itemNo);
        db->get_int(4, itemCheck);
        db->get_int(5, itemCnt);

        if (itemCheck == 0 && itemServerId == static_cast<int>(serverId))
        {
            rows[count][0] = static_cast<unsigned int>(rowId);
            rows[count][1] = static_cast<unsigned int>(itemCharacNo);
            rows[count][2] = static_cast<unsigned int>(itemNo);
            rows[count][3] = static_cast<unsigned int>(itemCnt);
            ++count;
        }
        ++scanned;
    }

    for (int i = 0; i < count; ++i)
    {
        UpdateEventMuzInfo(rows[i][0], static_cast<int>(rows[i][2]));
        EventCommon::InsertEventItem2EventTable(rows[i][1], rows[i][2],
                                                rows[i][3], 5);
    }
}

}  // namespace WongWork

// ---------------------------------------------------------------------------
// AvatarCoin::HistoryLog::SubLog —— ORIG T 0x0817ff54 (size 0x45)
// user!=0 → LogHistory("AvatarCoin-,%d,%d", amount, coin.Get())。
// coin 对象位于背包基址 +0x658（Arad_AvatarRoulette.cpp 同约定）。
// ---------------------------------------------------------------------------
void AvatarCoin::HistoryLog::SubLog(CUser* user, unsigned int amount)
{
    if (user == 0)
        return;
    // P0/P183：ORIG 无 `cur` 栈槽（Get() 结果 %eax 直传 LogHistory 出参槽）；
    // 具名局部致 spill/reload 2 条 + 帧 +0x10 → coin.Get() 内联为实参右值。
    user->LogHistory("AvatarCoin-,%d,%d", amount,
                     reinterpret_cast<AvatarCoin*>(
                         const_cast<char*>(reinterpret_cast<const char*>(
                             user->getCurCharacInvenRefR())) +
                         0x658)
                         ->Get());
}

// ---------------------------------------------------------------------------
// TOD_UserState::getLastEnterLayer —— ORIG T 0x08643942 (const, sret)
// ORIG 汇编：add $0x14 → TOD_Layer(TOD_Layer const&) 拷贝 ⇒ 读 +0x14
//（m_todayLayer，TOD_Layer 仅 ushort 字段，拷贝构造与 ushort 构造等价）。
// 头内字段已具名（DataManagerInitDepends.h TOD_UserState）。
// ---------------------------------------------------------------------------
TOD_Layer TOD_UserState::getLastEnterLayer() const
{
    return *(const TOD_Layer*)((const char*)this + 0x14);
}

// ---------------------------------------------------------------------------
// advanceAltarstatistics_ —— ORIG B @0x09400300 全局实例（构造/析构随全局
// __cxa 链）。唯一定义点；extern 声明在 CAdvanceAltarLog.h。
// ---------------------------------------------------------------------------
CAdvanceAltarLog advancealtar::HistoryLog::advanceAltarstatistics_;  // ORIG B 0x09400300

// ---------------------------------------------------------------------------
// CAutoMarketConditionsControl::GetItemDropRate —— ORIG T 0x082f8ae8
// （objdump 转写；报告缺失）。
// rate=1.0f；!m_field380 直接返回；levelRange 越界 → trace+返回 1.0f；
// v=m_vec3dc[level]，v!=0 时
// rate = 1.0f + (-1000.0/((m_equipProbBase-1.0)*v))（fldl/fdivrp 常量
// 0x8c1fc70=-1000.0 实证）；计数器 %1000 命中时打 itemDropRate trace。
// ---------------------------------------------------------------------------
// [b28d-D9] N95/N93：槽基差 -0xc vs -0x20、块级槽序；极性 P220（ORIG 0x082f8ae8 sz 0x000001a4）
float CAutoMarketConditionsControl::GetItemDropRate(int levelRange)
{
    float rate = 1.0f;
    if (m_field380 == 0)
        return rate;

    if (!(static_cast<int>(m_vec3dc.size()) > levelRange && levelRange >= 0))
    {
        cMyTrace trace(
            "double CAutoMarketConditionsControl::GetItemDropRate(int)", 0xde, 5);
        trace("%s error levelRange(%d)",
              "double CAutoMarketConditionsControl::GetItemDropRate(int)",
              levelRange);
        return rate;
    }

    int v = m_vec3dc[levelRange];
    if (v != 0)
    {
        double sub = m_equipProbBase - 1.0;
        rate = static_cast<float>(
            1.0 + (-1000.0 / (sub * v)));
    }

    static int s_traceCounter;                 // ORIG 匿名 b 计数器 0x0943bd78
    ++s_traceCounter;
    if (s_traceCounter % 1000 == 0)
    {
        double curRate = static_cast<double>(rate);
        cMyTrace trace(
            "double CAutoMarketConditionsControl::GetItemDropRate(int)", 0xe8, 5);
        trace("itemDropRate(%f), levelRange(%d), curRate(%lf)",
              curRate, levelRange, m_equipProbBase);
    }
    return rate;
}

// ---------------------------------------------------------------------------
// CAutoMarketConditionsControl::ChangeItemRarity —— ORIG T 0x082f8c8c
// （objdump 逐段转写；报告缺失）。m_field380==0 → false。
// out[2..5]=item[2..5]; i=2..0 向下：
//   delta=item[i+1]-item[i]; acc+=(int)(delta*factor)(round-to-nearest);
//   out[i]=item[i]-acc; if(out[i]<0) out[i]=0; 恒返 true。
// item/out 两表各取前 4 项打 trace（两次独立匿名计数器）。
// ---------------------------------------------------------------------------
// [b28d-D9] N95/N85：槽基差 -0x50 vs -0x60 + x87 fldl/fstpl 序列（ORIG 0x082f8c8c sz 0x000002b3）
bool CAutoMarketConditionsControl::ChangeItemRarity(int* itemRarity,
                                                    int* outItemRarity,
                                                    int raritySize,
                                                    double controlRarity)
{
    double factor = controlRarity;               // ebp+0x18/0x1c 双槽载入
    if (m_field380 == 0)
        return false;

    {                                            // 头部 trace（0x82f8cf4..）
        static int s_traceCounter;               // ORIG 0x0943bd70
        ++s_traceCounter;
        if (s_traceCounter % 1000 == 0)
        {
            cMyTrace trace(
                "bool CAutoMarketConditionsControl::ChangeItemRarity(int*,int*,int,double)",
                0xf4, 5);
            trace("%s itemRarity[%d,%d,%d,%d], outItemRarity[%d,%d,%d,%d], "
                  "itemRaritySize(%d), controlRarity(%lf)",
                  "bool CAutoMarketConditionsControl::ChangeItemRarity(int*,int*,int,double)",
                  itemRarity[0], itemRarity[1], itemRarity[2], itemRarity[3],
                  outItemRarity[0], outItemRarity[1], outItemRarity[2],
                  outItemRarity[3], raritySize, factor);
        }
    }

    for (int i = 2; i <= 5; ++i)
        outItemRarity[i] = itemRarity[i];

    int acc = 0;                                  // ebp-0x24
    for (int i = 2; i >= 0; --i)
    {
        int delta = itemRarity[i + 1] - itemRarity[i];
        acc += static_cast<int>(delta * factor);  // fnstcw RC=nearest 后 fistpl
        outItemRarity[i] = itemRarity[i] - acc;
        if (outItemRarity[i] < 0)
            outItemRarity[i] = 0;
    }

    {                                            // 尾部 trace（0x82f8ebf..）
        static int s_traceCounter;               // ORIG 0x0943bd74
        ++s_traceCounter;
        if (s_traceCounter % 1000 == 0)
        {
            cMyTrace trace(
                "bool CAutoMarketConditionsControl::ChangeItemRarity(int*,int*,int,double)",
                0x114, 5);
            trace("ChangeItemRarity outItemRarity[%d,%d,%d,%d]",
                  outItemRarity[0], outItemRarity[1], outItemRarity[2],
                  outItemRarity[3]);
        }
    }
    return true;
}
