// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LinkClosureB_Misc/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

// [L3FLinkClosureQuickParty 2026-09-11] §6.5.1 串直证：ORIG PF 串
// `void WongWork::EventATI::ArrangeATIEventItem(memberIdentificationNumber_t, unsigned int)`
// / EventMuz 同构，类型名**无限定** ⇒ typedef 必须在全局作用域（先例
// DB_Login.cpp:89 / CGuildServerProxy.cpp:151）。= unsigned int，mangled 不变。
typedef unsigned int memberIdentificationNumber_t;

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

#include "LinkClosureB_Misc/GetIdentity.inc"
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

// [b28d-solo-LinkClosureB_Misc] AE：P46 同值 case 合并为 fallthrough 标签组
//（4.4 不合并同体叶块）+ P286 内层 switch 形 + 照抄原码遗留重复 else-if。
#include "LinkClosureB_Misc/GetItemType.inc"
{
    int type = 0;
    if (eventKind == 2)
    {
        switch (itemNo)
        {
        // [b28d-solo-LinkClosureB_Misc] P46：ORIG 仅 4 个共享叶块（序 1/2/3/4）
        // ⇒ 同值 case 必须写成 fallthrough 标签组（4.4 不合并同体叶块）。
        case 0xae68:
        case 0x9ed0:
        case 0xbe0c:
        case 0xcd81:
            type = 1; break;
        case 0xa060:
        case 0xaff8:
        case 0xbf9c:
        case 0xcf11:
            type = 2; break;
        case 0x659d:
            type = 3; break;
        case 999:    // 0x3e7
            type = 4; break;
        }
    }
    else if (eventKind == 3)
    {
        // P286：ORIG 为 switch 形（形参载入 %eax + 寄存器比较），非 if 链。
        switch (itemNo)
        {
        case 0x659e: type = 1; break;
        case 8:      type = 2; break;
        }
    }
    else if (eventKind == 3)
    {
        // ORIG 原码遗留的重复分支（Ghidra 亦显示两次 param_1 != 3）。
        switch (itemNo)
        {
        case 1:      type = 1; break;
        }
    }
    return type;
}

}  // namespace WongWork

// ---------------------------------------------------------------------------
// WongWork::EventATI —— ORIG 事件族（报告 WongWork__EventATI/*）。
// 类与 DB_Login.cpp 的调用视图一致（全静态成员，无数据成员）。
// ---------------------------------------------------------------------------
namespace WongWork {

// [L3FLinkClosureQuickParty 2026-09-11] §6.5.1 串直证：ORIG 的 Event 族 PF 串无
// static（`void WongWork::EventATI::ArrangeATIEventItem(memberIdentificationNumber_t,
// unsigned int)`），而 ORIG 里另有 22 条 `static void NS::Class::method(...)` 串
// （实证 `static void DB_LogBuyItem::makeRequest(...)`）⇒ GCC 会给静态成员打印
// static。ORIG 的 Event 族是 **namespace**（非 class 静态成员）。改后 mangled 不变、
// 调用写法不变、PF 串去 static。Check*/Update* 不用 __PRETTY_FUNCTION__ ⇒ 无串影响。
namespace EventATI
{
    int CheckAlreadyGetATIItem(unsigned int uid, int itemNo);
    void UpdateEventATIInfo(unsigned int uid, int itemNo);
    void ArrangeATIEventItem(memberIdentificationNumber_t mid, unsigned int serverId);
}

// CheckAlreadyGetATIItem —— ORIG T 0x083f9f2e (size 0xd9)
// [b28d-D9] 修复：P46 叶块序：case1 写于 case2 前（交换后 .o AE 96/96）（ORIG 0x083f9f2e sz 0x0000017f）
#include "LinkClosureB_Misc/CheckAlreadyGetATIItem.inc"
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
#include "LinkClosureB_Misc/UpdateEventATIInfo.inc"
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
// [b28d-solo-LinkClosureB_Misc] N 封顶：语义/调用点已闭合（p167 24/24 CLEAR）。
// 已修：__PRETTY_FUNCTION__（P139）、dead local limit=10（P232）、P227 全表达式
// 临时链、P164/P283 for 底部条件 + unsigned 尾测 seta、P171 删 count<10 防御、
// 存储序 [0],[2],[3],[1]。残余 = P209（4.4 unsigned 形参比较恒载寄存器 +1/分支）
// + P311/N81 族（4.4 对 rows[count][1..2] 常量列下标折 scaled，4.1 恒 shl+lea+sub 链）。
// [F2e-Singles2 C05② 复核 2026-09-10] 两处 cmp 差（ORIG `mov -0x40/-0x54(%ebp),%eax;
//   cmp 0xc(%ebp),%eax` mem 直比）**比较对象是 unsigned 形参 serverId**（0xc(%ebp)）⇒
//   按 C05② 边界表 ❌ 形参子型，P209/N18b 封顶（改形参会动 mangled，禁）。主 gap 来源
//   仍是 P311 数组链（our 290 vs ORIG 307，3 处 shl$4+lea 链被 4.4 折 scaled）。
#include "LinkClosureB_Misc/ArrangeATIEventItem.inc"
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

}  // namespace WongWork

// ---------------------------------------------------------------------------
// WongWork::EventMuz —— ORIG 事件族（报告 WongWork__EventMuz/*）
// ---------------------------------------------------------------------------
namespace WongWork {

// [L3FLinkClosureQuickParty 2026-09-11] 同 EventATI：ORIG Event 族为 namespace。
namespace EventMuz
{
    int CheckAlreadyGetMuzItem(unsigned int uid, int itemNo);
    void UpdateEventMuzInfo(unsigned int uid, int itemNo);
    void ArrangeMuzEventItem(memberIdentificationNumber_t mid, unsigned int serverId);
}

// CheckAlreadyGetMuzItem —— ORIG T 0x083fb047 (size 0xba)
#include "LinkClosureB_Misc/CheckAlreadyGetMuzItem.inc"
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
#include "LinkClosureB_Misc/UpdateEventMuzInfo.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("upDate event_muz_entry set item_check=unix_timestamp(now()) where id=%s",
                  NumberToString(uid, 0));
    db->exec(true);
}

// ArrangeMuzEventItem —— ORIG T 0x083fb16a (size 0x240)
// [b28d-solo-LinkClosureB_Misc] N 封顶：同 ArrangeATIEventItem（p167 CLEAR）。
// 已修：__PRETTY_FUNCTION__、dead local limit=10、P227 临时链、for 底部条件、
// 存储序 [0],[2],[3],[1]。残余 = P209 + P311 数组常量列下标 scaled 折叠。
// [F2e-Singles2 C05② 复核 2026-09-10] 同上：`cmp 0xc(%ebp),%eax`（serverId 形参）⇒
//   N18b/P209 封顶；our 184 vs ORIG 194 gap +10 主体为 P311 scaled 折叠。
#include "LinkClosureB_Misc/ArrangeMuzEventItem.inc"
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

}  // namespace WongWork

// ---------------------------------------------------------------------------
// AvatarCoin::HistoryLog::SubLog —— ORIG T 0x0817ff54 (size 0x45)
// user!=0 → LogHistory("AvatarCoin-,%d,%d", amount, coin.Get())。
// coin 对象位于背包基址 +0x658（Arad_AvatarRoulette.cpp 同约定）。
// ---------------------------------------------------------------------------
#include "LinkClosureB_Misc/SubLog.inc"
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
#include "LinkClosureB_Misc/getLastEnterLayer.inc"
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
// [b28d-solo-LinkClosureB_Misc] NEAR：真语义 bug 修 3 处——(a) 除法方向应为
// `(v*sub)/(-1000.0)`（原 `-1000.0/(v*sub)` 整段反，ORIG fdivp 实证）；
// (b) 漏第二次 operator[]（P183；p167 MISSING×1 → CLEAR）；(c) 日志格式串多
// %s+重复签名实参、cMyTrace 第三参 5→0。另补 __PRETTY_FUNCTION__（P139）、
// logCount 符号名（P187）+ 后置自增序。残余 = N82附/N93/N95（4.4 聚合对象优先
// 低槽 + 标量分区 vs 4.1 生命周期穿插 ⇒ 槽号整体平移）。
#include "LinkClosureB_Misc/GetItemDropRate.inc"
{
    float rate = 1.0f;
    // [b28d-solo-LinkClosureB_Misc] P59/P257 正向包裹：ORIG `test; je <统一返回块>`
    //（m_field380==0 直落尾块，无 mov+jmp 提前返回岛）。
    if (m_field380 != 0)
    {
        if (!(static_cast<int>(m_vec3dc.size()) > levelRange && levelRange >= 0))
        {
            // [b28d-solo-LinkClosureB_Misc] P118/P139：ORIG 首参是 __PRETTY_FUNCTION__
            // 局部静态符号（_ZZ...E19__PRETTY_FUNCTION__）；格式串为单参版
            //（原重建多写 %s + 重复签名实参 = 真 bug）。
            cMyTrace trace(__PRETTY_FUNCTION__, 0xde, 0);
            trace("GetItemDropRate() error levelRange(%d)", levelRange);
            return rate;
        }

        // [b28d-solo-LinkClosureB_Misc] P183/P157：ORIG 两次独立 operator[]（p167
        // MISSING×1 实证）——原重建缓存 `int v` 少一次调用点。
        if (m_vec3dc[levelRange] != 0)
        {
            // ORIG fsubrp 实证 = `1.0 - m_equipProbBase`（非 base-1.0）。
            // ORIG fdivp 实证 = `(v*sub)/(-1000.0)`（非 -1000.0/(v*sub)，原重建
            // 除法方向整段写反 = 真语义 bug）。
            // 4.4 对标量 double 局部折叠为 `fmul mem`（少 1 条）；数组元素左值
            // 才发 ORIG 的 `fld mem; fmulp`（探针 p3.cpp f9）。
            double sub[1];
            sub[0] = 1.0 - m_equipProbBase;
            rate = static_cast<float>(
                1.0 + (m_vec3dc[levelRange] * sub[0]) / -1000.0);
        }

        // [b28d-solo-LinkClosureB_Misc] ORIG 符号名 _ZZ...E8logCount（P187）；
        // 后置自增形 `logCount++ % 1000 == 0` 才复现「先 sete 物化、后 lea 自增写回」。
        static int logCount;                   // ORIG b 0x0943bd78
        if (logCount++ % 1000 == 0)
        {
            // P0：ORIG 在 cMyTrace ctor 前先把 m_equipProbBase / (double)rate 各落具名
            // double 槽（-0x50/-0x48），trace 实参从槽读（非 ctor 后读成员）。
            double base = m_equipProbBase;
            double curRate = static_cast<double>(rate);
            cMyTrace trace(__PRETTY_FUNCTION__, 0xe8, 0);
            trace("itemDropRate(%f), levelRange(%d), curRate(%lf)",
                  curRate, levelRange, base);
        }
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
// [b28d-solo-LinkClosureB_Misc] N 封顶：语义/调用点闭合（p167 CLEAR）。
// 已修：__PRETTY_FUNCTION__、logCount/logCount2 符号名（P187）+ 后置自增序、
// cMyTrace 第三参 0、格式串删多余 %s、P191 i 双初始化 + 两循环复用同槽、
// P59 正向包裹、P227 全表达式临时链。残余 = N85（4.4 对 double 局部拷贝恒
// fld/fstp 中转，ORIG 恒 2×mov；探针 /tmp/b28d_lcbm/p4.cpp 六形全败）⇒ 帧 +0x10。
#include "LinkClosureB_Misc/ChangeItemRarity.inc"
                                                    int* outItemRarity,
                                                    int raritySize,
                                                    double controlRarity)
{
    // ORIG 为两条 mov 直拷（无 fld/fstp 中转、无中间临时槽 ⇒ 帧少 0x10）。
    // [L3FLinkClosureQuickParty 2026-09-11] 负结论：五形实测均无法消除 4.4 的
    // double 拷贝中转（均为 214→221/218 或与声明+赋值形同）：①`double factor =
    // controlRarity;` ②`const double factor = controlRarity;` ③P246/P180 u64 别名
    // 位拷贝（221，多 lea/mov 对）④`__builtin_memcpy(&factor,&controlRarity,8)`
    // （221，退化为 memcpy 调用序）⑤`*(double*)&factor = *(const double*)&controlRarity;`
    // （218）。⇒ N85 封顶（4.4 对 double 局部拷贝恒 `mov→临时槽 + fld/fstp`）。
    double factor;
    factor = controlRarity;                      // ebp+0x18/0x1c 双槽载入
    // P59/P257 正向包裹：ORIG `test; je <mov $0 尾块>`，成功路径 mov $1 fall-through。
    if (m_field380 != 0)
    {
      {                                          // 头部 trace（0x82f8cf4..）
        // P187：ORIG 符号 _ZZ...E8logCount；后置自增形复现 sete/lea 序。
        static int logCount;                     // ORIG b 0x0943bd70
        if (logCount++ % 1000 == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xf4, 0)(
                "ChangeItemRarity itemRarity[%d,%d,%d,%d], "
                "outItemRarity[%d,%d,%d,%d], "
                "itemRaritySize(%d), controlRarity(%lf)",
                itemRarity[0], itemRarity[1], itemRarity[2], itemRarity[3],
                outItemRarity[0], outItemRarity[1], outItemRarity[2],
                outItemRarity[3], raritySize, factor);
        }
      }

      // P191/P160-162：ORIG i 双初始化（`movl $0,-0x20` 后 `movl $2,-0x20`），
      // 且两个循环复用同一 i/acc 槽。
      int acc = 0;                                // ebp-0x24
      int i = 0;
      for (i = 2; i <= 5; ++i)
          outItemRarity[i] = itemRarity[i];

      for (i = 2; i >= 0; --i)
      {
          int delta = itemRarity[i + 1] - itemRarity[i];
          acc += static_cast<int>(delta * factor); // fnstcw RC=nearest 后 fistpl
          outItemRarity[i] = itemRarity[i] - acc;
          if (outItemRarity[i] < 0)
              outItemRarity[i] = 0;
      }

      {                                          // 尾部 trace（0x82f8ebf..）
        static int logCount2;                    // ORIG b 0x0943bd74
        if (logCount2++ % 1000 == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x114, 0)(
                "ChangeItemRarity outItemRarity[%d,%d,%d,%d]",
                outItemRarity[0], outItemRarity[1], outItemRarity[2],
                outItemRarity[3]);
        }
      }
      return true;
    }
    return false;
}
