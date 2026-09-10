// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPartyDepsA2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CParty.h"

#include <algorithm>
#include <stdio.h>
#include <vector>

#include "CDungeon.h"               // get_index/get_min_level/get_standard_level
#include "CDataManager.h"           // G_CDataManager/find_dungeon/m_conditionDungeon
#include "DataManagerInitDepends.h" // TowerOfDespairMgr/TOD_Layer/TOD_UserState
#include "TimerQueue.h"             // G_TimerQueue/TimerQueue::InsertTimer
#include "GlobalData.h"             // GlobalData::s_pTowerOfDespairMgr
#include "CWorldMap.h"              // IsInHellDungeon/m_field1c/m_field34
#include "PacketGuard.h"            // PacketGuard（InterfacePacketBuf 子类）
#include "CInventory.h"             // check_item_exist/delete_item 完整类型
#include "cMyTraceNoop.h"

namespace
{

// ORIG：user+0xcac 为当前 CDungeon*。CUser 该偏移区未建模（无具名字段/访问器），
// 按 DisPatcherCore.cpp 对 party+0xcac 的只读视图同口径处理【推断：布局口径】。
struct CUserCurDungeonView
{
    char m_pad[0xcac];
    CDungeon* m_dungeon;
};

}  // namespace

// ORIG 全局 _Z33getHellPartyEntranceItemNeedCounti（T 0x08891be6），
// 定义在 CParty.cpp；此处仅声明供 UseSealDoorItems 调用。
#include "CPartyDepsA2/getHellPartyEntranceItemNeedCount.inc"

// ORIG T 0x0822ec40 _ZNK9TOD_Layer8GetLayerEv：返回首 ushort。
#include "CPartyDepsA2/GetLayer.inc"
{
    return m_layer;
}

// ORIG T 0x086439c8 _ZNK9TOD_LayerplEi（报告缺失，按 nm 反汇编实现）：
// 构造 ret(m_layer+n)；ret.GetLayer()>100 → resetLayer()；sret 返回。
TOD_Layer TOD_Layer::operator+(int n) const
{
    TOD_Layer ret(static_cast<unsigned short>(m_layer + n));
    if (ret.GetLayer() > 100)
    {
        ret.resetLayer();
    }
    return ret;
}

// ============================================================================
// ConditionLevelChkDungeon::isEventDungeon —— ORIG T 0x089139d4
//   （报告 ConditionLevelChkDungeon/isEventDungeon.md）
//   m_unableDungeons 为空或不含 dungeonIdx → true。
// ============================================================================
// [b28c-31] AE：正向 `if (*it == dungeonIdx) return false;` 复现 ORIG
// 0x08913a2c 的 `sete %al; test; je <++it>` 正极性物化（46=46）。
#include "CPartyDepsA2/isEventDungeon.inc"
{
    if (m_unableDungeons.empty())
    {
        return true;
    }
    std::vector<int>::iterator it = m_unableDungeons.begin();
    std::vector<int>::iterator endIt = m_unableDungeons.end();
    while (it != endIt)
    {
        // [b28c-31] ORIG 0x08913a2c：`cmp 0xc(%ebp),%eax; sete %al; test %al,%al;
        // je <++it>`（正极性 sete 物化）⇒ 正向 if 返回形，勿写 !(a==b)（4.4 折 setne）。
        if (*it == dungeonIdx)
        {
            return false;
        }
        ++it;
    }
    return true;
}

// ============================================================================
// ConditionLevelChkDungeon::isApplyLevel —— ORIG T 0x08913a62
//   （报告 ConditionLevelChkDungeon/isApplyLevel.md）：m_applyLevel <= level。
// ============================================================================
#include "CPartyDepsA2/isApplyLevel.inc"
{
    if (!(m_applyLevel > level))
    {
        return true;
    }
    return false;
}

// ============================================================================
// TOD_UserState::getTodayEnterLayer —— ORIG T 0x0864383e
//   （报告 TOD_UserState/getTodayEnterLayer.md）：sret 返回
//   *(const TOD_Layer*)(this+0x14) + 1（经 TOD_Layer(ushort) 构造，>100 回卷 1；
//   构造回卷语义见 TOD_Layer/TOD_Layer.md 与 operator+ 尾部 resetLayer）。
// ============================================================================
#include "CPartyDepsA2/getTodayEnterLayer.inc"
{
    // ORIG：return *(const TOD_Layer*)(this+0x14) + 1
    //   （经 TOD_Layer::operator+(int)，sret 链与 ORIG 一致）；
    //   this+0x14 = TOD_UserState 内嵌 m_data.m_todayLayer（Sec 批 SIG 内嵌化，
    //   字段本体即 TOD_Layer。ORIG 0x864383e 单 call：operator+ 的 sret 直通
    //   本函数外层 sret（0x8(%ebp)），无临时/二段拷贝 ⇒ 源形为成员直 + 1。
    return m_data.m_todayLayer + 1;
}

// ============================================================================
// TowerOfDespairMgr::Get_CDungeonByLayer —— ORIG T 0x08643d14
//   （报告 TowerOfDespairMgr/Get_CDungeonByLayer.md）
// ============================================================================
// [b28d-Main·检查点17] 返回类型 CDungeon*→const CDungeon*（元素 const 指针，mangled 不变）。
#include "CPartyDepsA2/Get_CDungeonByLayer.inc"
{
    if (layer == 0 || 100 < layer)
    {
        return 0;
    }
    if (m_layerDungeons[layer] == 0)
    {
        m_layerDungeons[layer] =
            G_CDataManager()->find_dungeon(
                static_cast<int>(m_dungeonIndexOfLayer[layer]));
    }
    return m_layerDungeons[layer];
}

// ============================================================================
// TowerOfDespairMgr::Get_DungeonIndexByLayer —— ORIG T 0x08643d76
//   （报告 TowerOfDespairMgr/Get_DungeonIndexByLayer.md）
// ============================================================================
#include "CPartyDepsA2/Get_DungeonIndexByLayer.inc"
{
    if (layer == 0 || 100 < layer)
    {
        return 0;
    }
    return m_dungeonIndexOfLayer[layer];
}

// ============================================================================
// TowerOfDespairMgr::Get_APCIndexByLayer —— ORIG T 0x08643d9c
//   （报告 TowerOfDespairMgr/Get_APCIndexByLayer.md）：
//   层号在 m_npcLayers 中 → 返回层号；否则 -1；>100 → 0。
// ============================================================================
#include "CPartyDepsA2/Get_APCIndexByLayer.inc"
{
    if (100 < layer)
    {
        return 0;
    }
    std::vector<unsigned int>::iterator findIt =
        std::find(m_npcLayers.begin(), m_npcLayers.end(), layer);
    if (findIt != m_npcLayers.end())
    {
        return static_cast<int>(layer);
    }
    return -1;
}

// ============================================================================
// TowerOfDespairMgr::GetBaseLayerIndex —— ORIG T 0x086440a0
//   （报告 TowerOfDespairMgr/GetBaseLayerIndex.md）：m_field358 缓存基础层。
// ============================================================================
#include "CPartyDepsA2/GetBaseLayerIndex.inc"
{
    if (m_field358 == 0)
    {
        const CDungeon* base = Get_CDungeonByLayer(1);
        if (base != 0)
        {
            m_field358 = base->get_index();
        }
    }
    return m_field358;
}

// ============================================================================
// IsExistCharacter —— ORIG T 0x085b624e（报告 CParty/IsExistCharacter.md）
// [b28d-solo-CPartyDepsA2 判词·AE] 比较式交换操作数（call 在左）消除 4.4 跨
// call 缓存形参入 %ebx 的 push/pop 与帧差（P52/P0）。
// ============================================================================
#include "CPartyDepsA2/IsExistCharacter.inc"
{
    for (int local_10 = 0; local_10 <= 3; ++local_10)
    {
        if (!_checkValidUser(local_10))
            continue;
        // [b28d-solo-CPartyDepsA2] ORIG 0x085b62a8 段 `mov 0xc(%ebp),%edx;
        // cmp %edx,%eax`（call 后读形参入 edx）；把 call 放左操作数避免 4.4
        // 跨 call 缓存形参到 %ebx（P52/P0：左操作数先物化）。
        if (m_member[local_10].m_pUser->getCurCharacNo() == characNo)
        {
            return true;
        }
    }
    return false;
}

// ============================================================================
// FindUserByCharId —— ORIG T 0x085a2140（报告 CParty/FindUserByCharId.md）
//   返回命中槽的 CUser*（头文件签名 int；调用方按真值消费）。
// ============================================================================
#include "CPartyDepsA2/FindUserByCharId.inc"
{
    for (int local_10 = 0; local_10 <= 3; ++local_10)
    {
        if (!_checkValidUser(local_10))
            continue;
        if (static_cast<unsigned int>(
                m_member[local_10].m_pUser->get_charac_no(-1)) == characNo)
        {
            return reinterpret_cast<int>(m_member[local_10].m_pUser);
        }
    }
    return 0;
}

// ============================================================================
// BloodHandleDieUser —— ORIG T 0x085b8e5c（报告 CParty/BloodHandleDieUser.md）
// ============================================================================
// [b28d-53 P159] ORIG 尾 `mov $0x1,%eax`、错误路径 `mov $0x0,%eax` ⇒ 返回非 void；
// 声明随头改 bool（mangled 不含返回类型）。
#include "CPartyDepsA2/BloodHandleDieUser.inc"
{
    int local_10 = 0;
    for (local_10 = 0; local_10 <= 3; ++local_10)
    {
        // [b28d-53] ORIG 0x085b8e8c 段为正向包裹（`call; test %al,%al; je join`），
        // `if(!f()) continue;` 会多产 xor $1（P248 反向）。
        if (checkValidUser(local_10))
        {
            if (get_user(local_10) == user)
            {
                if (m_battleData.m_arr54[local_10] != 1)
                {
                    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x2b, 0x12);
                    return false;
                }
                set_charac_live(user, (ENUM_USER_DUNGEON_STATE)0);
                break;
            }
        }
    }

    if (check_allmember_die())
    {
        if (m_battleField.m_bloodState != 1)
        {
            // [b28d-53 P24/P52] ORIG 先求第 5 参 gen_timer_key 并入 %esi 跨后续
            // call 保活 ⇒ 实参内联（具名 key 局部落槽 +2 条并致帧 0x30→0x34）。
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                        GetPartyIndex(), (TIMER_MESSAGE)0x22,
                                        10, gen_timer_key((TIMER_MESSAGE)0x22),
                                        (unsigned int)0x22);
        }
    }
    return true;
}

// ============================================================================
// CheckClearQuestHellParty —— ORIG T 0x085a7a72（报告 CParty/CheckClearQuestHellParty.md）
//   out 清空后：对每个有效成员逐一核对 hell 任务清单（m_hellWorldMap+0x1c，
//   即 CWorldMap::m_field1c；无空检查，ORIG 如此）；存在未清任务 →
//   push_back 成员槽号并跳出该成员。
// [b28d-solo-CPartyDepsA2 判词·AE] 删 UserQuest* quest 具名局部（P0：ORIG 结果
// 直传 isClearQuest 的 this）+ 循环变量仅 for-init 初始化一次（ORIG 单条 movl）。
// ============================================================================
#include "CPartyDepsA2/CheckClearQuestHellParty.inc"
{
    out.clear();
    // [b28d-solo-CPartyDepsA2] ORIG 0x085a7a83 只有一条 `movl $0x0,-0x10`
    // （for-init 单次）⇒ 循环变量不在声明处初始化；且 ORIG 0x085a7b1b 段
    // getCurCharacQuestR 结果直传 isClearQuest 的 this、无 UserQuest* 具名局部
    // （P0 槽-局部律：我们的 -0xc 槽即该多出的局部）。
    for (int local_14 = 0; local_14 <= 3; ++local_14)
    {
        if (!_checkValidUser(local_14))
            continue;

        int questId = 0;
        std::vector<int>::iterator it = m_hellWorldMap->m_field1c.begin();
        std::vector<int>::iterator endIt = m_hellWorldMap->m_field1c.end();
        while (it != endIt)
        {
            questId = *it;
            if (!(m_member[local_14]
                      .m_pUser->getCurCharacQuestR()
                      ->isClearQuest(questId)))
            {
                out.push_back(local_14);
                break;
            }
            ++it;
        }
    }
}

// ============================================================================
// UseSealDoorItems —— ORIG T 0x085a8038（报告 CParty/UseSealDoorItems.md）
//
// [b28d-solo-CPartyDepsA2 判词·N 封顶] 185/185 条、助记符序列逐段同构；
// 已修：result/minLevel/itemId 具名局部删除（P0/P171）、非 const 迭代器
// （P96）、cMyTrace 全表达式临时链（P227）、slot 提到函数头、直接 return。
// 残余三族（语义等价）：
//  ① N95 槽基差 4：ORIG 槽基 local_14@-0x10/slot@-0x14/needCount@-0xc，
//     4.4 恒 ebp-0x18 起（帧顶多留 4B gap；double framePad 反致下移，实测）。
//  ② N88/P197④ 槽比 setne 物化：ORIG 0x085a8153 `cmpl $0xffffffff,-0x14;
//     setne %al; test %al,%al; je`（4.1 槽比零产物）；4.4-O0 对槽比恒出
//     `cmpl;je` 直跳——P197④ 探针实证源形不可复现（P225 while+break 形
//     要求单一条件+循环退出，本处 else 早返含 return false 无法单比较承载）。
//  ③ N7/N18b 寄存器角色差（slot 装载 eax↔edx）。
// ============================================================================
#include "CPartyDepsA2/UseSealDoorItems.inc"
{
    // [b28d-solo-CPartyDepsA2] ORIG 0x085a8042 函数头 `movl $0x0,-0x14`
    // （slot 声明处初始化，4.1 函数级）→ 0x085a8049 tmpItem 构造 →
    // 0x085a8054 local_14=0；ORIG 无 bool result 局部（尾 `mov $0x1,%eax`
    // 直返常量，P171 照抄：不写 result）⇒ slot 提到函数头、直接 return。
    int slot = 0;
    Inven_Item tmpItem;   // ORIG 栈上构造后未使用，保持一致

    for (int local_14 = 0; local_14 <= 3; ++local_14)
    {
        if (!_checkValidUser(local_14))
            continue;
        if (CheckHellDungeonFreepassItemHaveAndDel(m_member[local_14].m_pUser,
                                                   true))
            continue;

        // [b28d-solo-CPartyDepsA2] ORIG 0x085a80b3/0x085a8275 段 begin/end/
        // operator!= 均非 const 实例 ⇒ P96/P270 非 const iterator。
        std::vector<int>::iterator it = itemIds.begin();
        while (it != itemIds.end())
        {
            // [b28d-solo-CPartyDepsA2] ORIG 0x085a80de `call; cmp $0x2c,%eax;
            // setle`（无 minLevel 槽）⇒ 内联表达式直用（P0）；ORIG 0x085a8108
            // 二次调用 get_min_level 作 needCount 实参，故两处各自内联。
            if (m_battleField.m_pDungeon->get_min_level() <= 0x2c)
            {
                m_battleField.m_sealDoorItemsOk = 0;
                return false;
            }

            int needCount = getHellPartyEntranceItemNeedCount(
                m_battleField.m_pDungeon->get_min_level());
            // [b28d-solo-CPartyDepsA2] ORIG 0x085a8123 `mov (%eax),%ebx`：
            // *it 值经 ebx 直传 check_item_exist，无 itemId 具名局部。
            slot = m_member[local_14].m_pUser->getCurCharacInvenR()
                       ->check_item_exist(*it);
            if (slot != -1)
            {
                CInventory* invW = m_member[local_14].m_pUser->getCurCharacInvenW();
                if (!invW->delete_item((INVEN_TYPE)1, slot, needCount,
                                       (eItemDelReason)0x17, true))
                {
                    // [b28d-solo-CPartyDepsA2] ORIG 0x085a81d5：getCurCharacNo
                    // 在 cMyTrace ctor **之前**求值并入 %ebx 跨 ctor 保活 ⇒
                    // P227 全表达式临时链（具名 trace 反产 ctor 后求值 N80）。
                    cMyTrace(__PRETTY_FUNCTION__, 0x1d22, 5)(
                        "CParty::UseSealDoorItems, delete_item failed , "
                        "User ch =%d , %d %d",
                        m_member[local_14].m_pUser->getCurCharacNo(), slot,
                        needCount);
                }
                else
                {
                    m_member[local_14].m_pUser->SendUpdateItem(
                        (CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, slot);
                }
            }
            else
            {
                m_battleField.m_sealDoorItemsOk = 0;
                return false;
            }
            ++it;
        }
    }

    m_battleField.m_sealDoorItemsOk = 1;
    return true;
}

// ============================================================================
// CheckHaveHellPartyPassItem —— ORIG T 0x085a7b8a（报告 CParty/CheckHaveHellPartyPassItem.md）
// [b28d-solo-CPartyDepsA2 判词·AE] 负极性短路单 return `!A || B`（P214①/P258）
// + 成功分支正向 if（P59/P222）+ callee CheckDestroyConditionSealDoor 返回 bool
// （P292 2 条形）+ m_sealDoorFlag unsigned char + 本函数返回 unsigned char
// （P9 movzbl 直返；.h 三处已 R2 广播，CParty.cpp 定义已由 Main 代落同步）。
// ============================================================================
#include "CPartyDepsA2/CheckHaveHellPartyPassItem.inc"
                                                ENUM_DUNGEON_TYPE type)
{
    if (m_hellWorldMap == 0)
    {
        return true;
    }
    // [b28d-solo-CPartyDepsA2] ORIG 0x085a7bba 段：`call; xor $0x1; test %al;
    // jne <mov $1 岛>` + `cmpl $0x1,0x10(%ebp); je <mov $0 岛>` ⇒ 原码是负极性
    // 短路单 return `!A || B`（P214①/P258），非具名 bool skip——我们的
    // -0x9 槽 + movb 写即该多出的局部（P0 槽-局部律）。
    if (!m_hellWorldMap->IsInHellDungeon() || type != 1)
    {
        return true;
    }

    std::vector<int> sealCondItems;
    // [b28d-solo-CPartyDepsA2] ORIG 0x085a7c15 `call; test %al,%al;
    // je <packet 块>` ⇒ 成功分支（m_sealDoorFlag=1）在 then、packet 块为
    // 直落 else（P59/P222 正向布局），勿写 `if (!call())`。
    // [b28d-solo-CPartyDepsA2] ORIG 0x085a7c15 `call; test %al,%al; je`
    // （2 条，无 setne 物化）⇒ 被调返回 bool（P292），头已同步改 bool。
    if (CheckDestroyConditionSealDoor(dungeon, sealCondItems,
                                      m_hellWorldMap->m_field34))
    {
        m_sealDoorFlag = 1;
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x10);
        packet.put_byte(0);
        packet.put_byte(0x1b);
        packet.put_byte(static_cast<int>(sealCondItems.size()));
        // [b28d-solo-CPartyDepsA2] ORIG 0x085a7c8e 段 begin/end/operator!= 均
        // 非 const 实例（无转换构造 call）⇒ P96/P270 用非 const iterator。
        std::vector<int>::iterator it = sealCondItems.begin();
        while (it != sealCondItems.end())
        {
            packet.put_short(*it);
            ++it;
        }
        packet.finalize(true);
        send_to_party(packet);
        m_sealDoorFlag = 0;
    }
    return m_sealDoorFlag;
}

// ============================================================================
// SendPartyMemberTagChracInfo —— ORIG T 0x085be05a（报告 CParty/SendPartyMemberTagChracInfo.md）
// ============================================================================
#include "CPartyDepsA2/SendPartyMemberTagChracInfo.inc"
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x19e);

    int countIdx = packet.get_index();
    packet.put_short(0);

    int tagCount = 0;
    for (int local_14 = 0; local_14 <= 3; ++local_14)
    {
        CUser* user = get_user(local_14);
        // [b28d-53 归零] P223② 新实证：ORIG 两守卫共落同一 join（+0xa2）且 then
        // 尾有 `jmp <++i>; nop` 岛。复现形 = 扁平化「守卫1 continue + 守卫2 正向
        // if{…; continue;}」——嵌套 if 与负极性 `!call()` 均被 jump-thread/多产
        // xor+岛（实测 82/87 条）。此形 84=84 全等（ORIG 0x085be05a）。
        if (user == 0)
            continue;
        if (user->MakeTagCharacInfo(packet))
        {
            ++tagCount;
            continue;
        }
    }

    packet.put_short(countIdx, tagCount);
    packet.finalize(true);
    send_to_party(packet);
}

// ============================================================================
// GetDungeonIndex_isTOD —— ORIG T 0x085bacc0（报告 CParty/GetDungeonIndex_isTOD.md）
// [b28d-solo-CPartyDepsA2 判词·AE] 照抄写后未读 bool 局部（P171/P184）+ 删
// CDungeon* target 具名局部（P0）+ 删显式 cast（P211 LHS-first store）+
// cMyTrace/cMyTraceNoop 全表达式临时链（P221）+ Get_DungeonIndexByLayer 返回
// 类型 unsigned→int（.h 已 R2 广播）。
// ============================================================================
#include "CPartyDepsA2/GetDungeonIndex_isTOD.inc"
{
    // [b28d-solo-CPartyDepsA2] ORIG 0x085bacca 函数头 `movb $0x0,-0x19`、
    // 0x085bad2a 命中后 `movb $0x1,-0x19`（Ghidra local_1d 只写不读）⇒ 原码
    // 保留了写后未读的 bool 局部（P171/P184 照抄，勿删）。
    bool todFlag = false;
    TowerOfDespairMgr* tod =
        reinterpret_cast<TowerOfDespairMgr*>(GlobalData::s_pTowerOfDespairMgr);
    if (tod == 0)
    {
        // [b28d-solo-CPartyDepsA2] ORIG 0x085bace1 的 cMyTraceNoop 槽在 -0x2d
        // （cMyTrace -0x2c 之前，先声明更深）；具名对象 4.4 反产 -0x1a ⇒
        // P221 全表达式临时链翻 sibling 块槽向。
        cMyTraceNoop()("Error : DungeonIndex : %d", dungeonIdx);
        return 0x13;
    }

    if (tod->GetBaseLayerIndex() == dungeonIdx)
    {
        todFlag = true;
        CUser* manager = m_manager;   // ORIG this+0x74
        if (manager == 0)
        {
            // [b28d-solo-CPartyDepsA2] ORIG 0x085bad55 cMyTrace 槽 -0x2c、
            // cMyTraceNoop -0x2d、todayLayer -0x30 均在 msg 之后深侧；
            // 4.4 把具名对象反放浅侧 ⇒ P221 全表达式临时链（两 trace 同形）。
            cMyTrace(__PRETTY_FUNCTION__, 0x41bb, 5)("TOD no Dungeon Manager!");
            return 0x13;
        }

        TOD_UserState* state = reinterpret_cast<TOD_UserState*>(
            manager->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xd));
        TOD_Layer todayLayer = state->getTodayEnterLayer();

        // [b28d-solo-CPartyDepsA2] ORIG 0x085badf6 段 `mov 0xc(%ebp),%edx;
        // mov %eax,(%edx)`（LHS-first 直存）⇒ 原码无显式 cast（P211：显式
        // static_cast<int> 使 RHS 异型 ⇒ 4.4 反产 RHS-first + edx 中转）。
        dungeonIdx = tod->Get_DungeonIndexByLayer(todayLayer.GetLayer());

        // [b28d-solo-CPartyDepsA2] ORIG 0x085badc5 段：find_dungeon 结果经
        // %eax 直传 CheckEnterable 第三实参，无 CDungeon* target 具名局部
        // （P0 槽-局部律：我们的 -0x10 槽即该多出的局部）。
        int err = tod->CheckEnterable(manager,
                                      G_CDataManager()->find_dungeon(dungeonIdx));
        if (err != 0)
        {
            return err;
        }

        apcIdx = tod->Get_APCIndexByLayer(todayLayer.GetLayer());

        char msg[256];
        sprintf(msg, "Enter Layer : %d",
                static_cast<int>(todayLayer.GetLayer()));
        tod->SendMessageToCUser(manager, msg);
    }
    return 0;
}

// ============================================================================
// SetProperDungeonEvent —— ORIG T 0x085bb62a（报告 CParty/SetProperDungeonEvent.md）
// [b28d-solo-CPartyDepsA2 判词·AE] max/min 结果就地写回 minLevel/upperBound
// （P0：4.1 槽复用即源码复用）+ 先声明后赋值、声明序按 ORIG 槽序
// standardLevel/cap/zero/minLevel/upperBound（P198/P244），lower/upper 具名局部删除。
// ============================================================================
#include "CPartyDepsA2/SetProperDungeonEvent.inc"
{
    // [b28d-63] ORIG 0x085bb62a：两 bool 取反守卫（xor$1/test/jne）+ 两个
    // jl/jg 各落独立 nop 岛（0x736/0x739/0x73c/0x73f）⇒ P223 多岛形；
    // dungeon 取 **this->m_battleField.m_pDungeon**（ORIG `0x8(%ebp)+0xcac`），
    // user 经 CUser 基类接口直调（ORIG `0xc(%ebp)` 三次）；param 未被 ORIG 使用。
    user->ClearProperDungeonUser();

    if (!G_CDataManager()->m_conditionDungeon.isEventDungeon(
            m_battleField.m_pDungeon->get_index()))
        goto END1;
    if (!G_CDataManager()->m_conditionDungeon.isApplyLevel(
            user->get_charac_level()))
        goto END2;
    {
        // [b28d-solo-CPartyDepsA2] ORIG 槽序（浅→深）standardLevel -0x10、
        // cap -0x14、zero -0x18、minLevel -0x1c、upperBound -0x20、level -0xc；
        // max/min 结果就地写回 minLevel/upperBound 槽 ⇒ 先声明后赋值 +
        // 声明序按 ORIG 槽序（P198 孪生/P244 具名局部判据）。
        int standardLevel;
        int cap;
        int zero;
        int minLevel;
        int upperBound;
        int level;

        standardLevel = m_battleField.m_pDungeon->get_standard_level();
        minLevel = m_battleField.m_pDungeon->get_min_level();
        zero = 0;
        minLevel = std::max(minLevel, zero);
        upperBound = standardLevel + 1;
        cap = 0x46;
        upperBound = std::min(upperBound, cap);
        level = user->get_charac_level();
        if (level < minLevel)
            goto END3;
        if (level > upperBound)
            goto END4;
        user->SetProperDungeonUser();
    }
END1:;
END2:;
END3:;
END4:;
}

// ============================================================================
// SetProperLevelDungeon —— ORIG T 0x085bb746（报告 CParty/SetProperLevelDungeon.md）
// [b28d-solo-CPartyDepsA2 判词·AE] 同 SetProperDungeonEvent：max/min 就地写回
// + 先声明后赋值、声明序按 ORIG 槽序（P198/P244）。
// ============================================================================
#include "CPartyDepsA2/SetProperLevelDungeon.inc"
{
    user->ClearProperLevelDungeonUser();

    // [b28d-63] ORIG 0x085bb781：`xor $1; test; jne <nop 岛>` + 后段两独立
    // `jl/jg` 各落独立岛（0xe1/0xe4）⇒ P223 多岛形：负条件 goto + 尾标签串。
    if (!G_CDataManager()->m_conditionDungeon.isEventDungeon(
            m_battleField.m_pDungeon->get_index()))
        goto END1;
    {
        // [b28d-solo-CPartyDepsA2] 同 SetProperDungeonEvent：ORIG 槽序
        // standardLevel -0x10 / cap -0x14 / zero -0x18 / minLevel -0x1c /
        // upperBound -0x20 / level -0xc，max/min 就地写回 ⇒ 先声明后赋值。
        int standardLevel;
        int cap;
        int zero;
        int minLevel;
        int upperBound;
        int level;

        standardLevel = m_battleField.m_pDungeon->get_standard_level();
        minLevel = m_battleField.m_pDungeon->get_min_level();
        zero = 0;
        minLevel = std::max(minLevel, zero);
        upperBound = standardLevel + 1;
        cap = 0x46;
        upperBound = std::min(upperBound, cap);
        level = user->get_charac_level();
        if (level < minLevel)
            goto END2;
        if (level > upperBound)
            goto END3;
        user->SetProperLevelDungeonUser();
    }
END1:;
END2:;
END3:;
}
