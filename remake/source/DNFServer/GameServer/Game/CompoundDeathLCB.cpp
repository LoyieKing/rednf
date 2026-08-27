// ============================================================================
// CompoundDeathLCB.cpp — WongWork::CCompoundAvatar 合成核心四方法 +
//                        WongWork::CDeathTowerRanking 记录 packet（LinkClosureB）
//
// 全部实现逐段转写自 docs/class_func_reports/{WongWork__CCompoundAvatar,
// WongWork__CDeathTowerRanking}/ 报告汇编+反编译 C：
//   WongWork::CCompoundAvatar
//     _MakeCompoundInfo            T 0x08331efe（sret 返回 tagCompoundInfo）
//     _CheckCondition              T 0x0833260c（const）
//     _ProcCompoundCore            T 0x08332ee8
//     _SendDisjointAvatarStatistic T 0x08333ecc
//     _generateIPGNo               T 0x08333df6（const；闭链 helper，报告同目录）
//   WongWork::CDeathTowerRanking
//     makeMyBestRecordPacket       T 0x08468b8e
//     makeTopRankerRecordPacket    T 0x08468c68
//     _makeRecordPacket            T 0x08468cb6（前两者共用的序列化 helper）
// 同 TU 附加直接依赖 producer（报告完整、当前全源无定义）：
//   getItemIndex(const std::vector<std::pair<int,int>>&, int) T 0x08332e4e
//   Packet_Avater_Disjoint_Statistic C1 W 0x08110bae（内嵌 PacketHeader W 0x08110b70）
//   DB_UpdateAvatarHistory::makeRequest(unsigned int,int,char*) T 0x0844536a
// ============================================================================

#include "CCompoundAvatar.h"
#include "CDeathTowerRanking.h"
#include "CInventory.h"         // CInventory 完整类型 + OS_API::snprintf
#include "PacketGuard.h"        // PacketGuard 完整类型（CDeathTowerRanking.h 仅前向）
#include "CAvatarItemMgr.h"      // IsTempKey/GetRemainDate/GetExpireDate/UnRegistItem
#include "CUser.h"               // CUserCharacInfo getter/getCurCharacInven*
#include "GlobalData.h"          // s_systemTime_/s_statistic_proxy_mgr/s_stream_pool/s_msgq_mgr
#include "CSystemTime.h"
#include "MsgQueueMgr.h"        // MsgQueueMgr::put / QUEUE_IDX
#include "CServerProxyMgr.h"     // CServerProxyMgr<CStatisticServerProxy> 完整模板
#include "CStatisticServerProxy.h"
#include "CStreamGuard.h"
#include "GameTypes.h"          // cMyTrace
#include "CDataManager.h"        // G_CDataManager()/find_item
#include "PvPTypes.h"            // CMTRand（m_rand 完整类型）
#include "GameRand.h"            // get_rand_int
#include "DB_UpdateAvatarHistory.h"
#include <cstring>
#include <ctime>
#include <utility>
#include <vector>


// ---------------------------------------------------------------------------
// getItemIndex —— ORIG T 0x08332e4e _Z12getItemIndexRKSt6vectorISt4pairIiiESaIS1_EEi。
// 全局函数；唯一定义已在 HeroMissionLink4.cpp:257（签名/mangled 与 ORIG 一致），
// 此处仅声明复用，避免重复符号。语义：返回向量中第一个 second > bound 的
// 元素 first；遍历完仍无 → 0。
// ---------------------------------------------------------------------------
int getItemIndex(const std::vector<std::pair<int, int> >& vec, int bound);

// ---------------------------------------------------------------------------
// DB_UpdateAvatarHistory —— ORIG T 0x0844536a（makeRequest.md 反编译转写）。
// 流头 0x2b0 + -1；正文 characNo(uint)/avatarUid(int)/ipgNo 二进制 0x18 字节；
// 入 MsgQueue 队列 2。
// ---------------------------------------------------------------------------
void DB_UpdateAvatarHistory::makeRequest(unsigned int characNo, int avatarUid,
                                         char* ipgNo)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xac3b);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x2b0);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(static_cast<int>(characNo));
    guard.operator*()->operator<<(avatarUid);
    guard.operator*()->put_binary(ipgNo, 0x18);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---------------------------------------------------------------------------
// Packet_Avater_Disjoint_Statistic —— 统计包（本 TU 唯一定义；body 字段偏移按
// ORIG _SendDisjointAvatarStatistic 写入序建模，sizeof==0x28）。
// PacketHeader ctor W 0x08110b70：(u16 type@+0,u16 size@+2,u16 0@+4,dword 0@+6)。
// 权威 class PacketHeader 见 source/shared/packet/include/PacketHeader.h
// （0xa 头部布局与 ctor(type,size) 一致，经既有 include 链可见）。
// ---------------------------------------------------------------------------
#pragma pack(push, 1)

class Packet_Avater_Disjoint_Statistic : public PacketHeader
{
public:
    // ORIG W 0x08110bae：PacketHeader(0x17a2, 0x28) 后仅清零 dword @+0xa。
    Packet_Avater_Disjoint_Statistic()
        : PacketHeader(static_cast<unsigned short>(0x17a2),
                       static_cast<unsigned short>(0x28))
    {
        m_field0a = 0;
    }

    unsigned int m_field0a;       // +0x0a（调用方写 2）
    int m_gradeIdxA;              // +0x0e rarity==2?2:grade-1
    int m_subTypeA;               // +0x12 scriptA vtbl+0xc 结果
    unsigned char m_flagA;        // +0x16 置 0
    unsigned int m_pad17;         // +0x17..+0x1a 未初始化（与 ORIG 一致）
    int m_gradeIdxB;              // +0x1b 同 A 组算法但取 scriptB grade
    int m_subTypeB;               // +0x1f【ORIG 怪癖】也读 scriptA vtbl+0xc
    unsigned char m_flagB;        // +0x23 置 0
};                                // sizeof == 0x28
#pragma pack(pop)

namespace WongWork {

// ---------------------------------------------------------------------------
// ORIG CItem 虚表槽直调 helper。
// ORIG CItem vtable(.rodata 0x08c96da0) 槽序实证：D1/D2、D0、纯虚区；
//   +0x0c=子类型索引（派生 _ZNK10CEquipItem12get_sub_typeEv）、
//   +0x10=IsAvatarItem(bool)、+0x24=getUsableItemType(int)(bool)。
// 当前 CItem.h 虚函数声明序与该权威槽序的审计差异尚未收敛（AuditCItemVtable
// 域负责）；为保证本 TU 与 ORIG 虚分派逐指令等价，经槽偏移显式调用，
// 全局虚表收敛后应替换回语义虚调用。
// ---------------------------------------------------------------------------
static int CI_SubType(const void* item)
{
    typedef int (*Fn)(const void*);
    return (*reinterpret_cast<Fn*>(*reinterpret_cast<void* const*>(item) + 0x0c))(
        item);
}
static bool CI_IsAvatarItem(const void* item)
{
    typedef bool (*Fn)(const void*);
    return (*reinterpret_cast<Fn*>(*reinterpret_cast<void* const*>(item) + 0x10))(
        item);
}
static bool CI_UsableType1(const void* item)
{
    typedef bool (*Fn)(const void*, int);
    return (*reinterpret_cast<Fn*>(*reinterpret_cast<void* const*>(item) + 0x24))(
        item, 1);
}

// ============================ _MakeCompoundInfo ============================

// ORIG 0x08331efe。步骤：
//   1. 构造临时 tagCompoundInfo 并 clear()（ctor/clear 定义于 WongWorkMiscB_A.cpp）；
//   2. 回填 msg 定长字段（msg 结构未建模，按 ORIG 偏移读取）；
//   3. GetInvenSlot 三次取物品拷贝（slot1→m_avatar、slot2→材料A/B）；
//   4. find_item 两脚本指针；
//   5. 双脚本有效且 itemId 与槽内 addInfo 一致时，按 msg->type 在 rec 的
//      tail 向量链上找 first==avatar.add_info 的表项 rec 下标 → info.m_field04，
//      并落 info.m_type。
CCompoundAvatar::tagCompoundInfo CCompoundAvatar::_MakeCompoundInfo(
    const CUser* user, const MSG_COMPOUND_AVATAR* msg) const
{
    // msg 为未建模包结构（头内仅前置声明），字段按 ORIG 偏移读取：
    //   +0x0d slotA(u16→field90) / +0x0f slotB(u16→field08/materialA) /
    //   +0x11 itemIdA(i32) / +0x15 slotC(u16→field48/materialB) / +0x17 itemIdB(i32)
    //   +0x1b fieldD0(i32) / +0x1f slot(u16,d4) / +0x21 atkOptFlag(u8:==1→d6) /
    //   +0x22 type(u8,e8)
    const char* raw = reinterpret_cast<const char*>(msg);

    tagCompoundInfo info;                        // ORIG C1 0x08334344
    info.clear();                                // ORIG clear 0x0833424a

    info.m_fieldD6 = (*reinterpret_cast<const unsigned char*>(raw + 0x21) == 1);
    info.m_fieldD0 = *reinterpret_cast<const int*>(raw + 0x1b);
    info.m_slot = *reinterpret_cast<const unsigned short*>(raw + 0x1f);
    info.m_field90 = *reinterpret_cast<const unsigned short*>(raw + 0x0d);

    const CInventory* inv = user->getCurCharacInvenR();
    info.m_avatar = inv->GetInvenSlot(
        1, static_cast<int>(*reinterpret_cast<const short*>(raw + 0x0d)));
    info.m_field08 =
        *reinterpret_cast<const unsigned short*>(raw + 0x0f);
    info.m_materialA = inv->GetInvenSlot(
        2, static_cast<int>(*reinterpret_cast<const short*>(raw + 0x0f)));
    info.m_field48 =
        *reinterpret_cast<const unsigned short*>(raw + 0x15);
    info.m_materialB = inv->GetInvenSlot(
        2, static_cast<int>(*reinterpret_cast<const short*>(raw + 0x15)));

    info.m_scriptA = G_CDataManager()->find_item(
        *reinterpret_cast<const int*>(raw + 0x11));
    info.m_scriptB = G_CDataManager()->find_item(
        *reinterpret_cast<const int*>(raw + 0x17));

    if (info.m_scriptA == 0 || info.m_scriptB == 0)
    {
        info.m_errorFlag = true;                 // ORIG +0x2ef
        return info;
    }
    // 材料必须正是槽内两件物品（itemId 与 addInfo 相等校验）。
    if (*reinterpret_cast<const int*>(raw + 0x11) !=
            static_cast<int>(info.m_materialA.get_add_info()) ||
        *reinterpret_cast<const int*>(raw + 0x17) !=
            static_cast<int>(info.m_materialB.get_add_info()))
    {
        info.m_errorFlag = true;                 // ORIG +0x31b
        return info;
    }

    const tagCompoundAvatarTable& table = m_tables[info.m_field00];
    unsigned int found = 0;                      // -0x28：匹配到的 rec 下标
    for (unsigned int idx = 1; idx < table.m_avatars.size(); ++idx)  // -0x20
    {
        const tagCompoundAvatar* rec = table.getCompoundAvatarInfo(
            static_cast<int>(idx));
        if (rec == 0)
            continue;

        const std::vector<std::pair<int, int> >* opts = 0;
        switch (raw[0x22])
        {
            case 0:
                opts = &rec->m_tailA;            // +0x2b8
                break;
            case 1:
                opts = &rec->m_tailB;            // +0x2c4
                break;
            case 3:
                opts = &rec->m_tailC;            // +0x2d0
                break;
            case 4:
                opts = &rec->m_cube.m_vec;       // +0x20c
                break;
            default:
                continue;                        // 其它类型跳过本轮
        }
        for (std::vector<std::pair<int, int> >::const_iterator it = opts->begin();
             it != opts->end(); ++it)
        {
            if (it->first ==
                static_cast<int>(info.m_avatar.m_addInfo))
            {
                found = idx;                     // 匹配 → 记录 rec 下标并终止外层
                break;
            }
        }
        if (found != 0)
            break;
    }
    info.m_field04 = static_cast<int>(found);    // ORIG +0x6cd
    info.m_type = *reinterpret_cast<const unsigned char*>(raw + 0x22);  // +0x6d9
    return info;
}

// ============================== _CheckCondition =============================

// ORIG 0x0833260c（const）。错误码：1 材料异常/条目缺失、0x11 通用条件失败、
// 0x13 有效期类别不一致、0x16 缺事件材料、0x17 组合约束失败；0 通过。
int CCompoundAvatar::_CheckCondition(const CUser* user,
                                     const tagCompoundInfo& info) const
{
    // 分离模式(type4)仅允许指定物品 0x2898c4。
    if (info.m_type == 4 && static_cast<int>(info.m_avatar.m_addInfo) != 0x2898c4)
        return 0x11;
    if (info.m_slot > 99)                                      // u16 cmp 0x63
        return 0x11;
    if (info.m_errorFlag != 0)
        return 0x11;
    if (info.m_field00 == 0xb)                                 // 表索引无效
        return 0x17;
    if (info.m_materialA.get_add_info() == 0 ||
        info.m_materialB.get_add_info() == 0)
        return 0x11;

    // 任一材料的 uid(addInfo2) 是临时 key → 1。
    if (CAvatarItemMgr::IsTempKey(info.m_materialA.m_addInfo2) ||
        CAvatarItemMgr::IsTempKey(info.m_materialB.m_addInfo2))
        return 1;

    if (!CI_IsAvatarItem(info.m_scriptA) || !CI_IsAvatarItem(info.m_scriptB))
        return 0x11;
    if (CI_SubType(info.m_scriptA) != CI_SubType(info.m_scriptB))
        return 0x17;
    if (info.m_field04 == 0)
        return 0x17;
    if (!CI_UsableType1(info.m_scriptA) || !CI_UsableType1(info.m_scriptB))
        throw static_cast<ENUM_ERROR>(0x17);                   // __cxa_throw ENUM_ERROR

    const tagCompoundAvatarTable& table = m_tables[info.m_field00];
    const tagCompoundAvatar* rec =
        table.getCompoundAvatarInfo(info.m_field04);           // 所选 recipe 条目
    const int subType = CI_SubType(info.m_scriptA);
    const int gradeA = info.m_scriptA->get_grade();
    const int gradeB = info.m_scriptB->get_grade();

    // 非“加攻标记”时才检查选项池里存在用户请求的 option 对：
    // subType==rec->m_idx 用 m_optsC[sub]，否则 grade 必须 == rec->m_flag2dc
    // 再用 m_optsB[sub]。
    if (info.m_fieldD6 != 1)
    {
        const std::vector<std::pair<int, int> >* opts = 0;
        if (gradeA == rec->m_idx)
        {
            opts = &rec->m_optsC[subType];                     // +0x194
        }
        else
        {
            if (gradeA != static_cast<int>(rec->m_flag2dc))    // +0x2dc
                return 1;
            opts = &rec->m_optsB[subType];                     // +0xf4
        }
        bool found = false;
        for (std::vector<std::pair<int, int> >::const_iterator it = opts->begin();
             it != opts->end(); ++it)
        {
            if (it->first == info.m_fieldD0)
            {
                found = true;
                break;
            }
        }
        if (!found)
            return 1;
    }

    // grade 必须都 >= 所选 recipe 下标，且不超出表内上限 flag2dc。
    if (gradeA < info.m_field04 || gradeB < info.m_field04)
        return 0x17;
    if (static_cast<int>(rec->m_flag2dc) < gradeA ||
        static_cast<int>(rec->m_flag2dc) < gradeB)
        return 0x17;

    // 角色职业两件均可用（check_job_type 返回值与 1 比较）。
    const char job = static_cast<char>(user->get_charac_job());
    if (!(info.m_scriptA->check_job_type(job) == 1 &&
          info.m_scriptB->check_job_type(job) == 1))
        return 0x17;

    // 按类型验证事件材料消耗单。
    const CInventory* inv = user->getCurCharacInvenR();
    switch (info.m_type)
    {
        case 0:
            if (inv->CheckNeedItemList(rec->m_tailA) != 1)     // +0x2b8
                return 0x16;
            break;
        case 1:
            if (inv->CheckNeedItemList(rec->m_tailB) != 1)     // +0x2c4
                return 0x16;
            break;
        case 3:
            if (inv->CheckNeedItemList(rec->m_tailC) != 1)     // +0x2d0
                return 0x16;
            break;
        case 4:
            if (inv->CheckNeedItemList(rec->m_cube.m_vec) != 1)  // +0x20c
                return 0x16;
            break;
        default:
            break;
    }

    const int curSec = GlobalData::s_systemTime_.getCurSec();
    const CAvatarItemMgr* mgrR = inv->GetAvatarItemMgrR();
    const int remainA =
        mgrR->GetRemainDate(static_cast<int>(info.m_materialA.m_addInfo2), curSec);
    const int remainB =
        mgrR->GetRemainDate(static_cast<int>(info.m_materialB.m_addInfo2), curSec);
    const bool noExpireA =
        mgrR->GetExpireDate(static_cast<int>(info.m_materialA.m_addInfo2)) == 0;
    const bool noExpireB =
        mgrR->GetExpireDate(static_cast<int>(info.m_materialB.m_addInfo2)) == 0;

    if (noExpireA != noExpireB)
        return 0x13;
    if (noExpireA || (remainA > 5 && remainB > 5))
    {
        // 分离升级位标记禁拆者不允许操作。
        if (info.m_materialA.m_upgradeSep.IsTradeRestriction() ||
            info.m_materialB.m_upgradeSep.IsTradeRestriction())
            return 0x17;
        return 0;
    }
    return 0x18;
}

// ============================== _ProcCompoundCore ===========================

// ORIG 0x08332ee8。结果选择（按 rec 内三组权重池抽样）→ 删除两件材料与事件
// 材料 → 注销两 uid 并 push 结果三元组 → AddAvatarItem 入包 → 回填 DB 历史。
int CCompoundAvatar::_ProcCompoundCore(CUser* user, tagCompoundInfo& info,
                                       tagCompoundAvatarTable* table)
{
    CInventory* inv = user->getCurCharacInvenW();
    const tagCompoundAvatar* rec =
        table->getCompoundAvatarInfo(info.m_field04);
    const int subType = CI_SubType(info.m_scriptA);
    int resultIdx = 0;                                         // -0x48

    if (info.m_fieldD6 == 0 || info.m_type == 4)
    {
        if (info.m_type == 4)
        {
            const int r = get_rand_int(
                rec->m_cube.m_field0c[subType]);               // +0x218 权重计数
            resultIdx = getItemIndex(rec->m_cube.m_vecs[subType], r);  // +0x240
        }
        else
        {
            resultIdx = info.m_fieldD0;                        // 直接选定结果
        }
    }
    else
    {
        // 输出权重池空则失败。
        if (rec->m_b[subType] == 0 || rec->m_optsA[subType].empty())
            return 1;

        int weight = 0;                                        // -0x38
        if (info.m_scriptA->get_grade() == 1 &&
            info.m_scriptB->get_grade() == 1)
        {
            weight = rec->m_d[subType];                        // +0x16c
            if (m_rand->randInt(
                    static_cast<unsigned long>(10000)) < 5000) // setbe 0x1387
            {
                const int r = m_rand->randInt(
                    static_cast<unsigned long>(weight));
                resultIdx = getItemIndex(rec->m_optsC[subType], r);  // +0x194
            }
            if (resultIdx < 1)
                weight = rec->m_a[subType];                    // +0x004 兜底权重
        }
        else
        {
            weight = rec->m_e[subType];                        // +0x2e0
        }

        if (resultIdx < 1)
        {
            // 服务端全局修正（datamgr+0x4a54 加成 / +0x4a58 千分比衰减；
            // CDataManager 域具名化后改为成员访问）。
            if (info.m_type == 1)
            {
                weight += *reinterpret_cast<const int*>(
                    reinterpret_cast<const char*>(G_CDataManager()) + 0x4a54);
            }
            if (info.m_type == 3)
            {
                weight -= (*reinterpret_cast<const int*>(
                               reinterpret_cast<const char*>(G_CDataManager()) +
                               0x4a58) *
                           weight) /
                          10000;
            }
            if (m_rand->randInt(static_cast<unsigned long>(10000)) < weight)
            {
                const int r = get_rand_int(rec->m_b[subType]);
                resultIdx = getItemIndex(rec->m_optsA[subType], r);  // +0x7c
            }
        }
    }

    if (resultIdx == 0)
    {
        const int r = get_rand_int(rec->m_c[subType]);         // +0x054 权重计数
        resultIdx = getItemIndex(rec->m_optsB[subType], r);    // +0xf4
    }

    if (resultIdx != 0 && G_CDataManager()->find_item(resultIdx) != 0)
    {
        // ---- 成功路径：删两件材料 ----
        if (inv->delete_item((INVEN_TYPE)2,
                             static_cast<int>(info.m_field08), 1,
                             static_cast<eItemDelReason>(3), true) == 1 &&
            inv->delete_item((INVEN_TYPE)2,
                             static_cast<int>(info.m_field48), 1,
                             static_cast<eItemDelReason>(3), true) == 1)
        {
            bool bFoundAll = false;                            // -0x8d
            // 事件材料整单删除；type4 时 size 比较仍用 tailC【ORIG 怪癖】。
            const std::vector<std::pair<int, int> >* materials = 0;
            switch (info.m_type)
            {
                case 0:
                    materials = &rec->m_tailA;                 // +0x2b8
                    break;
                case 1:
                    materials = &rec->m_tailB;                 // +0x2c4
                    break;
                case 3:
                    materials = &rec->m_tailC;                 // +0x2d0
                    break;
                case 4:
                    materials = reinterpret_cast<
                        const std::vector<std::pair<int, int> >*>(
                        &rec->m_cube);                         // +0x20c 首成员即 m_vec
                    break;
                default:
                    break;
            }
            if (materials != 0)
            {
                const int deleted = inv->delete_event_items(
                    *materials, info.m_results, bFoundAll,
                    static_cast<eMoneySubReason>(0x12),
                    static_cast<eItemDelReason>(0x12), false);
                const std::size_t expectSize =
                    (info.m_type == 4) ? rec->m_tailC.size()
                                       : materials->size();
                if (deleted != static_cast<int>(expectSize))
                    return 0x16;
            }

            const int curSec = GlobalData::s_systemTime_.getCurSec();
            const CAvatarItemMgr* mgrR = inv->GetAvatarItemMgrR();
            const int remainA =
                mgrR->GetRemainDate(static_cast<int>(info.m_materialA.m_addInfo2),
                                    curSec);
            const int remainB =
                mgrR->GetRemainDate(static_cast<int>(info.m_materialB.m_addInfo2),
                                    curSec);

            // 注销并广播移除事件。
            inv->GetAvatarItemMgrW()->UnRegistItem(
                static_cast<int>(info.m_materialA.m_addInfo2));
            inv->SendAvatarEvent(0x36,
                                 static_cast<int>(info.m_materialA.m_addInfo2),
                                 1, static_cast<int>(info.m_field08) + 10, 0);
            info.m_results.push_back(1);                       // 结果三元组①
            info.m_results.push_back(static_cast<int>(info.m_field08));
            info.m_results.push_back(1);
            inv->GetAvatarItemMgrW()->UnRegistItem(
                static_cast<int>(info.m_materialB.m_addInfo2));
            inv->SendAvatarEvent(0x36,
                                 static_cast<int>(info.m_materialB.m_addInfo2),
                                 1, static_cast<int>(info.m_field48) + 10, 0);
            info.m_results.push_back(1);                       // 结果三元组②
            info.m_results.push_back(static_cast<int>(info.m_field48));
            info.m_results.push_back(1);

            char ipgNoBuf[0x14];                               // -0xa5
            _generateIPGNo(user, curSec, ipgNoBuf);

            info.m_resultSlot = static_cast<unsigned short>(
                inv->AddAvatarItem(resultIdx, remainA + remainB, false,
                                   static_cast<char>(info.m_slot),
                                   static_cast<char>(-1), ipgNoBuf,
                                   static_cast<eAvatarItemAddReason>(6), true,
                                   0));
            if (static_cast<short>(info.m_resultSlot) == -1)
            {
                cMyTrace trace(
                    "ENUM_ERROR WongWork::CCompoundAvatar::_ProcCompoundCore(CUser*, "
                    "WongWork::CCompoundAvatar::stCompoundAvatarInfo&, "
                    "stCompoundAvatarTable*)",
                    0x361, 5);
                trace("[%s][%d] AddAvatarItem Fail",
                      "ENUM_ERROR WongWork::CCompoundAvatar::_ProcCompoundCore(CUser*, "
                      "WongWork::CCompoundAvatar::stCompoundAvatarInfo&, "
                      "stCompoundAvatarTable*)",
                      0x361);
                return 0x11;
            }
            DB_UpdateAvatarHistory::makeRequest(
                static_cast<unsigned int>(user->getCurCharacNo()),
                static_cast<int>(info.m_materialA.m_addInfo2), ipgNoBuf);
            DB_UpdateAvatarHistory::makeRequest(
                static_cast<unsigned int>(user->getCurCharacNo()),
                static_cast<int>(info.m_materialB.m_addInfo2), ipgNoBuf);
            return 0;
        }
        return 0x16;                                           // delete_item 失败
    }
    return 1;                                                  // 结果不可实例化
}

// =========================== _SendDisjointAvatarStatistic ===================

// ORIG 0x08333ecc。统计服上报本次分离的两件头像分级；注意 B 组 subType 也读
// scriptA 的 vtbl+0xc（ORIG 如此）。
void CCompoundAvatar::_SendDisjointAvatarStatistic(tagCompoundInfo& info)
{
    Packet_Avater_Disjoint_Statistic packet;

    const CItem* scriptA = info.m_scriptA;
    const CItem* scriptB = info.m_scriptB;

    packet.m_field0a = 2;                                      // -0x36/local_36
    packet.m_gradeIdxA =
        (scriptA->get_rarity() == 2) ? 2 : scriptA->get_grade() - 1;
    packet.m_subTypeA = CI_SubType(scriptA);
    packet.m_flagA = 0;
    packet.m_gradeIdxB =
        (scriptB->get_rarity() == 2) ? 2 : scriptB->get_grade() - 1;
    packet.m_subTypeB = CI_SubType(scriptA);                   // 【ORIG 怪癖】保留
    packet.m_flagB = 0;

    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0);
    proxy->SendPacket(reinterpret_cast<char*>(&packet),
                      static_cast<int>(sizeof(packet)));       // 0x28
}

// ================================ _generateIPGNo ============================

// ORIG T 0x08333df6（const）。"C%02d%02d%02d%02d%010d" 五元组。
char* CCompoundAvatar::_generateIPGNo(const CUser* user, long curSec,
                                      char* buf) const
{
    time_t t = static_cast<time_t>(curSec);
    struct tm lt;
    localtime_r(&t, &lt);
    const int characNo = user->getCurCharacNo();
    // CEnvironment 服务器标识 +0x1b0（该域具名化后改为成员访问）。
    const int serverId =
        *reinterpret_cast<const volatile int*>(
            reinterpret_cast<const char*>(G_CEnvironment()) + 0x1b0);
    OS_API::snprintf(buf, 0x14, "C%02d%02d%02d%02d%010d",
                     serverId,
                     (lt.tm_year + 1900) % 2000,
                     lt.tm_mon + 1,
                     lt.tm_mday,
                     characNo);
    return buf;
}

}  // namespace WongWork

// ======================== CDeathTowerRanking packets ========================

namespace WongWork {

// ORIG T 0x08468cb6。每名成员：strlen 名字(int)+名字串、两个槽位 byte
// （stBestRecord_t +0x80/+0x84 即 m_pad7c[4+i]/m_pad7c[8+i]）、层号 short、
// 通关耗时 int、名次 int。
void CDeathTowerRanking::_makeRecordPacket(const stBestRecord_t& record,
                                           PacketGuard& packet)
{
    for (int i = 0; i < 4; ++i)
    {
        const int len = static_cast<int>(strlen(record.m_memberData[i]));
        packet.put_int(len);
        packet.put_str(const_cast<char*>(record.m_memberData[i]), len);
        packet.put_byte(record.m_pad7c[4 + i]);                // +0x80
        packet.put_byte(record.m_pad7c[8 + i]);                // +0x84
    }
    packet.put_short(static_cast<int>(record.m_stage));        // +0x88
    packet.put_int(static_cast<int>(record.m_playTime));       // +0x8c
    packet.put_int(record.m_field94);                          // +0x94
}

// ORIG T 0x08468b8e。查 best 表：命中 put_byte(1)+纪录；否则 put_byte(0)。
void CDeathTowerRanking::makeMyBestRecordPacket(unsigned int towerIdx,
                                                unsigned int key,
                                                PacketGuard& packet)
{
    BestMap& best = m_bestTables[towerIdx];                    // _getBestRecordTable
    BestMap::iterator it = best.find(key);
    if (it != best.end())
    {
        packet.put_byte(1);
        _makeRecordPacket(it->second, packet);
    }
    else
    {
        packet.put_byte(0);
    }
}

// ORIG T 0x08468c68。该塔 5 个榜首槽顺序序列化（基址 tower*0x390，stride 0x98）。
void CDeathTowerRanking::makeTopRankerRecordPacket(unsigned int towerIdx,
                                                   PacketGuard& packet)
{
    stBestRecord_t* slots = m_towers[towerIdx].m_slots;
    for (int i = 0; i < 5; ++i)
        _makeRecordPacket(slots[i], packet);
}

}  // namespace WongWork
