// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Dispatcher_CompoundEmblem_impl/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "Dispatchers.h"

#include <cstring>          // strlen
#include <utility>          // std::pair/make_pair
#include <vector>

#include "Inven_Item.h"
#include "CItem.h"
#include "CStackableItem.h"
#include "CDataManager.h"
#include "CInventory.h"
#include "CUserCharacInfo.h"
#include "PacketGuard.h"            // CUser::Send(PacketGuard&)
#include "CGameManager.h"           // WongWork::CMailBoxHelper
#include "RDARScriptStringManager.h"
#include "CompoundEmblemDeps.h"     // getCompoundEmblemStatistic / getAvatarEmblemTypeIdxForCompountEmblem / LineFunc
#include "Packet_Emblem_Create_Statistic.h"  // Packet_Emblem_Create_Statistic（C1 W 0x08110bdc）
#include "CStatisticServerProxy.h"
#include "GlobalData.h"             // GlobalData::s_statistic_proxy_mgr
#include "CServerProxyMgr.h"        // CServerProxyMgr<T>::GetServerProxy

// _selectJewelTypeIndex 与 SendCreateEmblemInfo 为 ORIG 私有成员
// （_ZNK25Dispatcher_CompoundEmblem21_selectJewelTypeIndexEjPKt T 0x08220a60、
//  _ZN25Dispatcher_CompoundEmblem20SendCreateEmblemInfoERKSt6vectorISt4pairIiiESaIS2_EE
//  T 0x08221dde），声明于 Dispatchers.h，本 TU 实现。
// ============================================================================

// ORIG _ZNK25Dispatcher_CompoundEmblem21_selectJewelTypeIndexEjPKt（0x8220a60）
// 纹章位图→槽位 加权随机选取珍宝类型索引。
#include "Dispatcher_CompoundEmblem_impl/_selectJewelTypeIndex.inc"
    unsigned int count, const unsigned short* sockets) const
{
    // b28d-40: N82/N93 封顶——帧 0x78 vs 0x74+push ebx + 具名数组槽序差，语义闭合。
    // ORIG 0x08220a60。
    int socketCounts[5] = {0, 0, 0, 0, 0};   // 各类型计数（ORIG local_54[6..10]）
    int accums[5]        = {0, 0, 0, 0, 0};   // 各类型累计权重（local_54[1..5]）
    int total            = 0;                 // 权重总和（local_54[0]）
    int cumulative       = 0;                 // 累计（local_54[0xb]）

    for (unsigned int i = 0; i < count; ++i)
    {
        if (sockets[i] != 0)
            getAvatarEmblemTypeIdxForCompountEmblem(sockets[i], socketCounts, total);
    }
    if (total != 0)
    {
        for (int j = 0; j < 5; ++j)
        {
            int weight = socketCounts[j] * (int)(10000 / total);
            accums[j]  = weight + cumulative;
            cumulative += weight;
        }
        int r = get_rand_int(cumulative);
        for (int k = 0; k < 5; ++k)
        {
            if (r < accums[k])
                return k;
        }
    }
    return -1;
}

// 布局（ORIG 汇编实证）：Packet_Emblem_Create_Statistic(0x17a4, 0x36)
// + count@slots[0](+0xa) + grades@slots[i+1](+0xe) + addInfo@slots[i+6](+0x22)，
// SendPacket(pkt, 0x36)。迭代器 while 循环（ORIG __gnu_cxx::operator!= 判续）。
#include "Dispatcher_CompoundEmblem_impl/SendCreateEmblemInfo.inc"
    const std::vector<std::pair<int, int> >& created)
{
    // b28d-40: N82 封顶——帧 0x74 vs 0x64 + 迭代器/局部槽平移，语义闭合。ORIG 0x08221dde。
    Packet_Emblem_Create_Statistic packet;

    // ORIG 0x08221dfb seta：size()>5 直接返回。
    if (created.size() > 5)
        return;
    // [b29 R20] 判据⑩/P28：ORIG 0x08221f05 `movl $0x0,-0x18`（count=0）出现在
    // begin(-0x20)/end(-0x24) 调用**之后** ⇒ count 声明后移至迭代器之后。
    std::vector<std::pair<int, int> >::const_iterator it = created.begin();
    std::vector<std::pair<int, int> >::const_iterator end = created.end();
    int count = 0;
    while (it != end)
    {
        // [b29 R20] ORIG 0x08221e3f/0x08221e4f 两次 operator->（P157 逐字段重读，
        // 每次落具名 int 槽 -0x14/-0x10）：key=it->first、value=it->second。
        int key = it->first;
        int value = it->second;
        const CItem* item = G_CDataManager()->find_item(key);
        if (item == 0)
            return;                       // ORIG 中途 return
        packet.m_slots[count + 1] = item->get_grade();
        packet.m_slots[count + 6] = value;
        ++count;
        if (count > 5)
            break;
        it++;                             // [b29 R20] P28 增量侧：ORIG 调 ppEi（后置）
    }
    // ORIG 0x08221ef4 setne：count != size 时返回。
    if (count != (int)created.size())
        return;
    packet.m_slots[0] = count;
    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0);
    proxy->SendPacket(reinterpret_cast<char*>(&packet), 0x36);
}
#include "Dispatcher_CompoundEmblem_impl/dispatch_sig.inc"
{
    // b28d-40: 真语义 bug 修正 3 处——① 11 次 CItem::get_grade() 使用点重读（原缓存
    // gradeA/B，P157/P183）；② invA/invB 校验改 m_addInfo（+0x02，ORIG 0x08220f54
    // 字段直读；原误用 get_add_info +0x07）；③ isEnableAvatarSocketAction 单调用
    // +具名槽（P0）。callsite 差集 158 vs 156；残 = N82 槽平移 + EH _Unwind_Resume
    // + make_pair 实参类型旋钮（受 CStackableItem.h 共享头限制）。ORIG 0x08220bb2。
    static const char SELF[] = "virtual int Dispatcher_CompoundEmblem::dispatch_sig(CUser*, PacketBuf&)";

    if (user->get_state() != 3)
        return 0;

    unsigned long itemIdxA;
    unsigned short slotA;
    unsigned long itemIdxB;
    unsigned short slotB;

    if (!packet.get_item_idx(itemIdxA))
        return LineFunc(0xda8f, SELF, 0, 0);
    if (!packet.get_short(slotA))
        return LineFunc(0xda90, SELF, 0, 0);
    if (!packet.get_item_idx(itemIdxB))
        return LineFunc(0xda91, SELF, 0, 0);
    if (!packet.get_short(slotB))
        return LineFunc(0xda92, SELF, 0, 0);

    Inven_Item invA;
    Inven_Item invB;
    CItem* itemA = G_CDataManager()->find_item((int)itemIdxA);
    CItem* itemB = G_CDataManager()->find_item((int)itemIdxB);
    int   jewelTypeIndex = -1;
    int   resultItemIdx  = 0;

    try
    {
        // b28d-40: P0——ORIG 0x08220d47 `call; mov %eax,-0x28(%ebp); cmpl $0,-0x28`
        // 有槽往返 ⇒ 原码是具名局部（原双调用形多发一次 call）。
        int err = user->isEnableAvatarSocketAction();
        if (err != 0)
            throw (ENUM_ERROR)err;

        if (itemA == 0 || itemB == 0)
            throw (ENUM_ERROR)0x11;

        // 材料必须为纹章材料（GetItemType()==0x14）才允许合成
        if (itemA->GetItemType() != 0x14 || itemB->GetItemType() != 0x14)
            throw (ENUM_ERROR)0x17;

        invA = user->getCurCharacInvenR()->GetInvenSlot(1, (int)slotA);
        if (invA.isEmpty())
            throw (ENUM_ERROR)0x11;
        // b28d-40: 真语义 bug 修正——ORIG 0x08220f54 比较的是 invA.m_addInfo
        // （+0x02，item idx，字段直读），原码误用 get_add_info()（+0x07 m_addInfo2）。
        if (invA.m_addInfo != (int)itemIdxA)
            throw (ENUM_ERROR)0x11;

        invB = user->getCurCharacInvenR()->GetInvenSlot(1, (int)slotB);
        if (invB.isEmpty())
            throw (ENUM_ERROR)0x11;
        if (invB.m_addInfo != (int)itemIdxB)
            throw (ENUM_ERROR)0x11;

        // 两槽位同 idx 且 add_info<2 非法
        if (slotA == slotB &&
            invA.m_addInfo == invB.m_addInfo &&
            invA.get_add_info() < 2)
            throw (ENUM_ERROR)0x11;

        if (user->CheckItemLock(1, (int)slotA))
            throw (ENUM_ERROR)0xd5;
        if (user->CheckItemLock(1, (int)slotB))
            throw (ENUM_ERROR)0xd5;

        // b28d-40: P157/P183——ORIG 0x08220bb2 段共 11 次 `CItem::get_grade()` 调用
        // （每次使用点重读，无 gradeA/gradeB 缓存局部）。
        if (itemA->get_grade() != itemB->get_grade())
            throw (ENUM_ERROR)0xd8;
        if (itemA->get_grade() < 0 || itemA->get_grade() > 6)
            throw (ENUM_ERROR)0x11;

        // 该纹章等级合成费用表：cost==-1 不可合成
        if (G_CDataManager()->m_serverParameter.m_emblemCompound[itemA->get_grade()].m_cost == -1)
            throw (ENUM_ERROR)7;
        if (itemB->get_grade() < 0 || itemB->get_grade() > 6)
            throw (ENUM_ERROR)0x11;
        if (G_CDataManager()->m_serverParameter.m_emblemCompound[itemB->get_grade()].m_cost == -1)
            throw (ENUM_ERROR)7;

        // 珍宝目标槽位（每个材料物品一槽）
        unsigned short jewelSockets[2];
        jewelSockets[0] = static_cast<CStackableItem*>(itemA)->getJewelTargetSocket();
        jewelSockets[1] = static_cast<CStackableItem*>(itemB)->getJewelTargetSocket();
        jewelTypeIndex = _selectJewelTypeIndex(2, jewelSockets);
        if (jewelTypeIndex == -1)
            throw (ENUM_ERROR)0x17;

        // 合成结果物品（按 grade 与珍宝类型查表）
        resultItemIdx = G_CDataManager()->m_serverParameter.m_emblemCompound[itemA->get_grade()].m_result[jewelTypeIndex];
        CStackableItem* resultItem =
            static_cast<CStackableItem*>(G_CDataManager()->find_item(resultItemIdx));
        if (resultItem == 0 || resultItem->GetItemType() != 0x10)
            throw (ENUM_ERROR)0x16;

        // 扣除合成费用
        int cost = G_CDataManager()->m_serverParameter.m_emblemCompound[itemA->get_grade()].m_cost;
        if (!user->getCurCharacInvenW()->use_money(cost, (eMoneySubReason)0x23, true))
            throw (ENUM_ERROR)0xa;

        // 删除两个材料道具（eItemDelReason 8）
        if (!user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, (int)slotA, 1,
                                                     (eItemDelReason)8, true))
            throw (ENUM_ERROR)1;
        if (!user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, (int)slotB, 1,
                                                     (eItemDelReason)8, true))
            throw (ENUM_ERROR)1;

        // 重新取两槽（删除后刷新）
        invA = user->getCurCharacInvenR()->GetInvenSlot(1, (int)slotA);
        invB = user->getCurCharacInvenR()->GetInvenSlot(1, (int)slotB);

        // ---- 回包 0x104：booster 道具列表 ----
        PacketGuard pg;
        pg.put_header(1, 0x104);
        pg.put_byte(1);

        std::vector<stStackableBoosterItemInfo_t> boosterItems;
        resultItem->getBoosterItem(boosterItems);
        pg.put_byte((int)boosterItems.size());
        for (std::vector<stStackableBoosterItemInfo_t>::const_iterator it =
                 boosterItems.begin(), end = boosterItems.end(); it != end; ++it)
        {
            pg.put_item_idx((unsigned long)it->m_itemIdx);
            pg.put_int((int)it->m_addInfo);
        }
        pg.finalize(true);
        user->Send(pg);

        // ---- 把 booster 结果道具插入背包（失败则系统邮件补发） ----
        std::vector<std::pair<int, int> > created;
        for (std::vector<stStackableBoosterItemInfo_t>::const_iterator it =
                 boosterItems.begin(), end = boosterItems.end(); it != end; ++it)
        {
            int itemIdx = it->m_itemIdx;
            CItem* itm = G_CDataManager()->find_item(itemIdx);
            if (itm == 0)
                continue;
            Inven_Item inv;
            itm->make_item(inv);
            inv.set_add_info((int)it->m_addInfo);

            short insertResult = (short)user->getCurCharacInvenW()->insertItemIntoInventory(
                inv, (eItemAddReason)9, true, true);
            if (insertResult < 0)
            {
                const char* title = g_scriptStringManager_.findString(4, "game_server_msg_122", 0);
                const char* body  = g_scriptStringManager_.findString(4, "game_server_msg_123", 0);
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    title, inv, 0, (unsigned int)user->getCurCharacNo(), body,
                    (int)std::strlen(body), 0, user->GetServerGroup(), false, false);
            }
            else
            {
                user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, (int)insertResult);
            }
            // b28d-40: P61——ORIG make_pair<unsigned int const&, unsigned short const&>
            // 由两个成员左值直传；原码 (int) cast 造成 <int&,int> 值化实例。
            created.push_back(std::make_pair(it->m_itemIdx, it->m_addInfo));
        }

        // ---- 统计 ----
        getCompoundEmblemStatistic()->increaseCount(itemA->get_grade());
        SendCreateEmblemInfo(created);
    }
    catch (ENUM_ERROR& e)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x104, (unsigned char)e);
        return 0;
    }

    return 0;
}

#include "Dispatcher_CompoundEmblem_impl/process.inc"
#include "Dispatcher_CompoundEmblem_impl/read.inc"
