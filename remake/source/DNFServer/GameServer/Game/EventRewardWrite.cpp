// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 EventRewardWrite/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// EventRewardWrite.cpp —— eventReward 奖励发放链（OrigStStructs 独占）。
//
// 覆盖 ORIG 真实符号（全部 T，报告 docs/class_func_reports/eventReward/*.md 全文转写）：
//   createCharacInvenReward 0x080f0270 / createCharacEquipReward 0x080f033c
//   rewardWriteUserInven    0x080f0408 / rewardWriteUserEquip    0x080f06da
//   sendPostalReward        0x080f0976 / unsealRandomOption      0x080f0e40
//   applyItemOption         0x080f0d6e / StartEndPos             0x080f0d22（报告缺失，
//   objdump 直转）
//
// 注：ORIG 中 createCharac* / sendPostalReward 的调用侧以
//   G_CDataManager()->m_eventReward（CDataManager+0xb628）作为接收者，本 TU 照此书写；
//   this 形参在这些函数中不参与发放（见各报告 Ghidra C）。

#include <string.h>
// ============================================================================
// b28d-52 判词表（本 TU）：
//   已修：rewardKey::operator< 0x080f121a —— P222 正向包裹 + P196 三字节字段
//     signed char（CDataManager.h，已 R2 广播）⇒ 转 AE。
// [b28d-D03 本波 判词]（fresh `check_tu_routed.sh`：identical+ae 56 / near 3 / diff 3，
//   非 identical 件 = unsealRandomOption + applyItemOption + sendPostalReward +
//   rewardWriteUserInven/Equip + _Z41__static_initialization…（N123 逐 TU 撞名豁免））
//   · unsealRandomOption 0x080f0e40 —— **DIFF→NEAR**：ORIG +0x3c `mov 0x10(%ebp),%eax;
//     add $0x25,%eax; mov %eax,-0x1c` ⇒ `&inven.m_random.m_random` 有**具名指针局部**且在
//     首个 getter **之前**物化（P0 槽往返 + P28 声明序）。补 `RandomOption* pOption`
//     并前置后槽位（-0x1c）与 ORIG 对齐；残差 = ①N7 虚调用目标 4.4.7 恒 %eax（ORIG %edx，
//     两处）②第 4 个 getter 值（rarity）4.4 落槽而 ORIG 留 %edx（register/具名 handle 两形
//     实测均劣：78/78→79）。P230：N7 属 4.4.6 后端敏感族，路由由 Main 统管。
//   · applyItemOption 0x080f0d6e —— 77→75 条：①`inven.m_field0` 经 uchar 视图直存
//     （消 4.4 `test;setne` bool 归一，且得 ORIG 的 RHS-first 序）②`if (({ …&&…; }))`
//     P226 语句表达式复现 ORIG +0x91..+0xaf 短路物化岛（mov $1/jmp/mov $0 + test）
//     ③`register unsigned int attachType` 消槽（P0/P231 非绑定形）。残差 = N6 位折叠
//     （ORIG `(1<<t)&0x27` shl 形被 4.4 折 `(0x27>>t)&1` sar）+ N7 + 1 条 LHS/RHS 序；
//     attachType 无槽值复用（ORIG `cmp $5,%eax` 复用同一 %eax 两次）合法 C++ 不可造。
//   · sendPostalReward 0x080f0976 —— 261 条（ORIG 259）：①谓词改读 `inven.m_addInfo`
//     （ORIG +0x2ab `mov -0x87(%ebp),%eax` = inven+0x02，非 itemIdx 副本）②P226 岛
//     ③lvl/len/characNo/titleStr 四值改**实参位内联 + `({ …; })`**（LBL8/P226）⇒ 4.4 分别
//     升格 %ebx/%esi/%ebx/%eax，复现 ORIG 无槽形。残差 = 2 条（ENUM_SERVER_GROUP cast 的
//     中转槽 -0x19c + N7 虚调）；expDate/varargs 求值序为跨版本（P75/P87）。
//   · rewardWriteUserInven 0x080f0408 / rewardWriteUserEquip 0x080f06da —— **NEAR**：
//     p167 调用点差集 0（除 C1/C2、D1/D2 同址别名伪差，P93）；唯一残差 = N7 单处
//     （`citem->make_item(inven)` 虚调 vptr+0x8 目标 4.4.7 恒 %eax，ORIG %edx）。
// ============================================================================

#include <map>
#include <string>
#include <vector>

#include "CDataManager.h"       // eventReward / rewardKey / itemData / G_CDataManager / find_item
#include "Inven_Item.h"         // Inven_Item
#include "CItem.h"              // CItem 虚/非虚访问器
#include "CInventory.h"         // g_slotBoundChecker / OS_API::snprintf
#include "CItemGenerator.h"     // random_option::CRandomOptionItemHandle::give_option / ENUM_RARITY
#include "GlobalData.h"         // g_randomOptionHandle
#include "LogManager.h"         // LogManager::logFormat
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString
#include "CMailBoxHelper.h"     // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "GameWorld.h"          // G_GameWorld / IsIntegratedPvPBaseChannel
#include "CEnvironment.h"       // G_CEnvironment
#include "CUser.h"              // CUser（继承 CUserCharacInfo：get_charac_level/getCurCharacNo）

namespace
{

// eventReward+0x18 的 CSlotBoundChecker 视图（P1 组件视图折叠为单 disp 寻址）。
struct EventRewardCheckerPos
{
    char m_pad[0x18];
    CSlotBoundChecker m_checker;
};

}  // unnamed namespace

// ORIG 以 this+0x18 内嵌 CSlotBoundChecker 调用 get_slot_bound；重建 CDataManager.h
// 以同布局 map 成员 m_slotBoundMap 保持 ABI（CSlotBoundChecker 静态实现无成员），
#include "EventRewardWrite/StartEndPos.inc"
                              int cap) const
{
    if (!reinterpret_cast<EventRewardCheckerPos const*>(this)->m_checker.get_slot_bound(
            type, cap, start, end))
    {
        start = 0;
        end = 0;
    }
}

// ============================================================================
// eventReward::applyItemOption —— ORIG T 0x080f0d6e
// ============================================================================
#include "EventRewardWrite/applyItemOption.inc"
                                  const itemData& data)
{
    if (item->is_stackable())
    {
        inven.m_addInfo2 = data.m_count;            // item+0x07 = data+0x08
        return;
    }
    reinterpret_cast<unsigned char&>(inven.m_field0) = data.m_seal;
    register unsigned int attachType = item->GetAttachType();  // ORIG 单次调用（寄存器化）
    // 负结论（4.4 位折叠族，N 类；本波五变体实测，75 为封顶最优）：
    //  ORIG 0x080f0da7–0x080f0dc3 = `call; cmp $5,%eax; ja` + `mov $1,%edx; mov %edx,%ebx;
    //  mov %eax,%ecx; shl %cl,%ebx; mov %ebx,%eax; and $0x27,%eax; test %eax,%eax`
    //  （attachType 直用 %eax，无具名寄存器副本）。
    //  4.4 恒折 `(1u<<x)&0x27` → `(0x27>>x)&1`（shr $cl + and $1）+ 多发 `mov %eax,%ebx`；
    //  实测：操作数序交换 75、`(int)`cast 75、有符号 `1<<x`（sar）75、嵌套 if 75、
    //  P226 语句表达式 80（语句内变量落 -0xc 槽 + setne 岛）⇒ 均不可消，74/75 封顶。
    if (attachType < 6 && ((1u << attachType) & 0x27))
    {
        inven.m_field0 = 0;
    }
    if (data.m_upgrade > 1)                         // +0x00 字节
    {
        inven.SetUpgrade(data.m_upgrade);
    }
    // ORIG 0x80f0d6e：虚调用 vptr+0x4c = vtable+0x54 = vt[21] IsRandomOption
    // （vptr 指向 vtable+8；勿与 vt[19] IsPossibleKiriProtection 混淆）
    // ORIG +0x91..+0xaf：短路物化岛（mov $1/jmp/mov $0 汇聚 + test）⇒ P226 语句表达式形。
    // ORIG +0x98 `movzbl 0x2(%eax),%eax; xor $0x1,%eax; test %al,%al; je`（取反物化）；
    // 实测 4.4 对 `== 0` 与 `!m_roi` 均产 `test %al,%al; jne` 直跳（无 xor）⇒ 二者
    // codegen 全同（75 条），保留 `== 0`。
    if (({ item->IsRandomOption() && data.m_roi == 0; }))  // +0x02 字节
    {
        this->unsealRandomOption(item, inven);
    }
}

// ============================================================================
// eventReward::unsealRandomOption —— ORIG T 0x080f0e40
// ============================================================================
#include "EventRewardWrite/unsealRandomOption.inc"
{
    if (item == 0)
        return 0;
    // ORIG 0x80f0e40：虚调用 vptr+0x4c = vtable+0x54 = vt[21] IsRandomOption
    if (!item->IsRandomOption())
        return 0;
    // ORIG 反汇编：第二次虚调用 vptr+0x50 = vtable 槽 20 = GetRandomOptionGrade
    //（CItem vtable @0x8c96da8：s19=IsRandomOption, s20=GetRandomOptionGrade）。
    // ORIG +0x3c..+0x42：&inven.m_random(+0x25) 在首个 getter 之前物化到 -0x1c 槽 ⇒ 具名指针局部。
    RandomOption* pOption = &inven.m_random.m_random;
    register int period = item->GetRandomOptionGrade();
    register int group = item->getItemGroupName();
    register int lvl = item->getUsableLevel();
    register int rarity = item->get_rarity();
    // 负结论（4.4 实参 temp 分配族，N 类；三次实测均劣于本形）：
    //  ORIG +0x57..+0x71 四值保活 edi/esi/ebx/edx（0x080f0e97/0x0ea4/0x0eb1/0x0ebe），
    //  +0x8b `mov &g_randomOptionHandle,%eax; mov %eax,-0x2c` ⇒ -0x2c 专存 handle，
    //  +0x71 `mov -0x1c(%ebp),%eax; mov %eax,0x18(%esp)` pOption 实参段直读。
    //  全内联 4 getter（指针形/引用形 0x080f0e7c–0x0f31）与「三局部+末位内联」
    //  均产 80 条：4.4 必为第 7 实参 pOption 另发 temp（-0x2c 存 pOption、handle 退
    //  -0x3c），恒多 2 条 mov ⇒ 不可由源码形态消除，78=78 为本件封顶。
    if (!GlobalData::g_randomOptionHandle->give_option(
            (unsigned long)inven.m_addInfo,
            (ENUM_RARITY)rarity, lvl, group, period, pOption))
    {
        LogManager::logFormat(
            1, "eventReward.cpp",
            __PRETTY_FUNCTION__, 0x240,
            "autoUnsealItem : %d", inven.m_addInfo);
    }
    return 1;
}

// ============================================================================
// eventReward::rewardWriteUserInven —— ORIG T 0x080f0408
// ============================================================================
#include "EventRewardWrite/rewardWriteUserInven.inc"
{
    if (this->m_map0.empty())
        return 0;
    std::map<rewardKey, std::map<int, std::vector<itemData> > >::iterator it =
        this->m_map0.find(key);
    if (it == this->m_map0.end())
        return 0;
    // [L10-2] p167 EXTRA `_ZNKSt17_Rb_tree_iterator…ptEv` +1 ⇒ 我方比 ORIG 多一次
    // `it->second`（4.4 每处 `it->` 各发一次 const ptEv）⇒ ORIG 为单次解引用缓存。
    std::map<int, std::vector<itemData> >& subMap = it->second;
    std::map<int, std::vector<itemData> >::iterator fit = subMap.find(count);
    if (fit == subMap.end())
        return 0;
    std::vector<itemData>& vec = fit->second;
    if (vec.empty())
        return 0;
    // [L10-2] P28 增量侧：ORIG 调 `ppEi`（后置）⇒ `vit++`。
    for (std::vector<itemData>::iterator vit = vec.begin(); vit != vec.end(); vit++)
    {
                // ORIG +0x11a..+0x134：*vit 取 m_itemIdx（ebx 跨 G_CDataManager 调用存活）
        // 直接嵌套实参；citem 判空后 +0x157 再 *vit 一次写 inven.m_addInfo。
        CItem* citem = G_CDataManager()->find_item(vit->m_itemIdx);
        if (citem == 0)
            return 0;
                Inven_Item inven;
        inven.m_addInfo = vit->m_itemIdx;       // ORIG +0x162/+0x165（make_item 前）
        citem->make_item(inven);                    // vtable+0x8
        applyItemOption(inven, citem, *vit);
        int start = 0;
        int end = 0;
        StartEndPos(static_cast<Inven_Item::ITEM_TYPE>(inven.m_field1),
                    start, end, 0);
        for (int i = start; i <= end; ++i)
        {
            if (item[i].m_addInfo == 0)             // +0x02 槽判空
            {
                item[i] = inven;                    // 整体 0x3d 字节写入
                break;
            }
        }
    }
    return 1;
}

// ============================================================================
// eventReward::rewardWriteUserEquip —— ORIG T 0x080f06da
// ============================================================================
#include "EventRewardWrite/rewardWriteUserEquip.inc"
{
    if (this->m_map0.empty())
        return 0;
    std::map<rewardKey, std::map<int, std::vector<itemData> > >::iterator it =
        this->m_map0.find(key);
    if (it == this->m_map0.end())
        return 0;
    // [L10-2] 同 rewardWriteUserInven：ORIG 单次解引用（消 p167 EXTRA const ptEv）。
    std::map<int, std::vector<itemData> >& subMap = it->second;
    std::map<int, std::vector<itemData> >::iterator fit = subMap.find(count);
    if (fit == subMap.end())
        return 0;
    std::vector<itemData>& vec = fit->second;
    if (vec.empty())
        return 0;
    for (std::vector<itemData>::iterator vit = vec.begin(); vit != vec.end(); vit++)
    {
                // ORIG +0x11a..+0x134：*vit 取 m_itemIdx（ebx 跨 G_CDataManager 调用存活）
        // 直接嵌套实参；citem 判空后 +0x157 再 *vit 一次写 inven.m_addInfo。
        CItem* citem = G_CDataManager()->find_item(vit->m_itemIdx);
        if (citem == 0)
            return 0;
                Inven_Item inven;
        inven.m_addInfo = vit->m_itemIdx;       // ORIG +0x162/+0x165（make_item 前）
        citem->make_item(inven);                    // vtable+0x8
        applyItemOption(inven, citem, *vit);
        if (citem->is_stackable())
            return 0;
        const int slot = citem->GetItemType();      // vtable+0xc
        item[slot - 10] = inven;                    // slot*0x3d - 0x262
    }
    return 1;
}

// ============================================================================
// eventReward::sendPostalReward —— ORIG T 0x080f0976
// ============================================================================
#include "EventRewardWrite/sendPostalReward.inc"
                                  const char* title, const char* body)
{
    if (this->m_map0.empty())
        return 0;
    std::map<rewardKey, std::map<int, std::vector<itemData> > >::iterator it =
        this->m_map0.find(key);
    if (it == this->m_map0.end())
        return 0;
    // [L10-2] 同 rewardWriteUserInven：ORIG 单次解引用（消 p167 EXTRA const ptEv）。
    std::map<int, std::vector<itemData> >& subMap = it->second;
    std::map<int, std::vector<itemData> >::iterator fit = subMap.find(count);
    if (fit == subMap.end())
        return 0;
    std::vector<itemData>& vec = fit->second;
    if (vec.empty())
        return 0;
    // [L10-2] P28 增量侧：ORIG 调 `normal_iterator::ppEi`（后置）⇒ 源形 `vit++`；
    // `++vit` 走 ppEv 会缺 ppEi 并多发 ppEv（p167 MISSING/EXTRA 实证）。
    for (std::vector<itemData>::iterator vit = vec.begin(); vit != vec.end(); vit++)
    {
        const int itemIdx = vit->m_itemIdx;
        Inven_Item inven;
        CItem* citem = G_CDataManager()->find_item(itemIdx);
        // [本波] serverGroup 声明为 ENUM_SERVER_GROUP（而非 int + 实参位 cast）：
        // ORIG 0x080f0c8c `mov -0x20(%ebp),%edx; mov %edx,0x1c(%esp)` 直读槽、实参位
        // 无额外临时 ⇒ 261→259 条（=ORIG）。残余 1 条：我方 4.4 在 0x080f0c39 之后多发
        // `mov -0x1c(%ebp),%edi`（expDate 提前保活），而 ORIG 0x080f0c93 是实参段直读
        // `mov -0x1c(%ebp),%edx`——属 4.4 实参保活族；`({ expDate; })` 包裹与去 const
        // 两变体实测零变化（均 259）。
        ENUM_SERVER_GROUP serverGroup = static_cast<ENUM_SERVER_GROUP>(0);
                // ORIG +0x159..+0x185：IsIntegratedPvPBaseChannel 先测，test %al 无取反；
        // 真分支（集成 PvP）取 user.GetServerGroup，假分支取 G_CEnvironment+0x378。
        if (G_GameWorld()->IsIntegratedPvPBaseChannel())
                        serverGroup = static_cast<ENUM_SERVER_GROUP>(user.GetServerGroup());
        else
            serverGroup = static_cast<ENUM_SERVER_GROUP>(
                G_CEnvironment()->m_serverEnvir.m_serverGroup);
        if (citem != 0)
        {
            inven.m_addInfo = itemIdx;              // item+0x02
            citem->make_item(inven);                // vtable+0x8
            applyItemOption(inven, citem, *vit);
            char buf[256] = { '\0' };
            // ORIG +0x243：`call get_charac_level; mov %eax,%ebx`（无槽，跨 findString 保活）
            // ⇒ P226 语句表达式实参（值升格 callee-saved）。
            OS_API::snprintf(buf, 0xff,
                             g_scriptStringManager_.findString(4, body, 0),
                             ({ user.get_charac_level(); }));
            // ORIG +0x2ab：读的是 inven.m_addInfo（-0x87 = inven+0x02），非 itemIdx 形参副本；
            // 且短路带 bool 物化岛（mov $1/jmp/mov $0 + test）⇒ P226 语句表达式。
            if (({ inven.m_addInfo != 0 && citem->is_stackable(); }))
            {
                const int expDate = citem->getExpirationDate();
                // ORIG +0x2c2..+0x2e6：strlen→%esi、getCurCharacNo→%ebx、title 串→%eax
                // 均无栈槽 ⇒ 实参位内联 + P226 语句表达式升格。
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    g_scriptStringManager_.findString(4, title, 0), inven, 0,
                    ({ user.getCurCharacNo(); }), buf, ({ (int)strlen(buf); }),
                    expDate, serverGroup, 0, 1);
            }
        }
    }
    return 1;
}

// ============================================================================
// eventReward::createCharacInvenReward —— ORIG T 0x080f0270
// ============================================================================
#include "EventRewardWrite/createCharacInvenReward.inc"
{
    bool ok = false;
    if (G_CDataManager()->m_eventReward.rewardWriteUserInven(item, key, count))
        ok = true;
    key.m_a = 0;                                    // +0x11
    key.m_sub.m_action = 1;                         // +0x14（inven）
    if (G_CDataManager()->m_eventReward.rewardWriteUserInven(item, key, count))
        ok = true;
    key.m_job = 0xb;                                // +0x10
    key.m_a = 0;                                    // +0x11
    key.m_sub.m_action = 1;                         // +0x14
    if (G_CDataManager()->m_eventReward.rewardWriteUserInven(item, key, count))
        ok = true;
    return ok;
}

// ============================================================================
// eventReward::createCharacEquipReward —— ORIG T 0x080f033c
// ============================================================================
#include "EventRewardWrite/createCharacEquipReward.inc"
{
    bool ok = false;
    if (G_CDataManager()->m_eventReward.rewardWriteUserEquip(item, key, count))
        ok = true;
    key.m_a = 0;                                    // +0x11
    key.m_sub.m_action = 2;                         // +0x14（equip）
    if (G_CDataManager()->m_eventReward.rewardWriteUserEquip(item, key, count))
        ok = true;
    key.m_job = 0xb;                                // +0x10
    key.m_a = 0;                                    // +0x11
    key.m_sub.m_action = 2;                         // +0x14
    if (G_CDataManager()->m_eventReward.rewardWriteUserEquip(item, key, count))
        ok = true;
    return ok;
}
