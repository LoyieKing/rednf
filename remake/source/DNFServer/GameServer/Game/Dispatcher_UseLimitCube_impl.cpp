// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Dispatcher_UseLimitCube_impl/<Func>.inc（不参与编译、内部不含 #include）。
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

// ============================================================================
// Dispatcher_UseLimitCube  —— 还原 ORIG（df_game_r，elf32-i386）
//
// 报告：docs/class_func_reports/Dispatcher_UseLimitCube/
//   check_error  0x081d3bbc  _ZN23Dispatcher_UseLimitCube11check_errorEijiR10Inven_ItemS1_PK5CItem
//   read         0x081d3c62  _ZN23Dispatcher_UseLimitCube4readER9PacketBufR8MSG_BASE
//   process      0x081d3d38  _ZN23Dispatcher_UseLimitCube7processEP5CUserR8MSG_BASER9ParamBase
//   send         0x081d4784  _ZN23Dispatcher_UseLimitCube4sendEP5CUserR9ParamBase
//
// 客户端「使用极限立方体（Limit Cube）」配方：把槽 A 的材料（itemIdx）
// 与槽 B 的立方体组合，产出配方结果物品。消息布局（MSG_BASE m_head[0xd] 之后）：
//   +0x0d short  slotA（材料槽）   +0x0f uint  itemIdx（材料 item idx）
//   +0x13 short  slotB（立方体槽）
// 参数布局（stUseLimitCube，ParamBase vptr@+0 之后 m_pad[0x40]）：
//   +0x04 uchar  error（0=成功，其余为错误码）
//   +0x08 int    resultIdx（结果物品索引）  +0x0c int resultCount
//
// 语义链（依 process.md 反编译，逐段核对）：
//   state==3 → 读 3 字段 → GetInvenSlot(1,slotA)=invA / GetInvenSlot(1,slotB)=invB
//   cube  = find_item(invB.m_addInfo)；source = find_item(invA.m_addInfo)
//   CheckItemLock(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)0), slotA) 非 0 → param.error=1
//   check_error(slotA,itemIdx,slotB,invA,invB,cube) → 非 0 → param.error=码
//   get_limit_cube_item(&pair) → {resultIdx, resultCount}；两者均 0 → error=0x11
//     param.resultIdx=pair.first；param.resultCount=pair.second
//   count1 = cube->get_1st_material_count(itemIdx)
//   mat2Idx = cube->get_2nd_material_index();  mat2Count = cube->get_2nd_material_count()
//   材料表 = {{itemIdx,count1},{invB.m_addInfo,1},{mat2Idx,mat2Count}}
//   CheckNeedItemList(材料表) 失败 → error=0x11
//   （临时库存 setCopy + tryDeleteEventItems 模拟删除，失败 → error=0x11）
//   source 非 stackable → 复制 invA（保留 ResealCount&0xff / add_info[+7] / m_attr[+6]）
//   三次 delete_item：
//     ① source：delete_item(1,slotA,count1,8,1)      → 更新元组 {0,slotA,count1}
//     ② cube：  delete_item(1,slotB,1,8,1)           → 更新元组 {0,slotB,1}
//     ③ mat2：  check_item_exist(mat2Idx) → slot；delete_item(1,slot,mat2Count,8,1)
//                                               → 更新元组 {0,slot,mat2Count}
//   更新元组逐组 SendUpdateItem(eSendTarget_1, ENUM_ITEMSPACE_0, slot)
//   resultDef = find_item(pair.first)      // 结果对象索引（corrected by Main）
//   resultDef->make_item(out)；返回 0 → error=0x11
//   out 非 stackable → 复制 invA 并 set_add_info；stackable → out.m_fieldd=resultCount
//   newSlot = insertItemIntoInventory(out, (eItemAddReason)9, true, true)
//     < 0 → error=0x11；否则 error=0
//   wrong-result 日志 guard（cube 0x1f06 → [0x671d,0x6749] 之外；0x1f11 → [0x6749,0x6775] 之外）
//   SendUpdateItem(eSendTarget_1, ENUM_ITEMSPACE_0, newSlot)
//
// 全部错误分支（error 置码后）返回值恒为 0（dispatch_template 不视为失败）。
// ============================================================================

#include "InterfacePacketBuf.h"   // InterfacePacketBuf（PacketGuard 基类，put_*）
#include "PacketGuard.h"          // PacketGuard 权威声明（CUser::Send(PacketGuard&) 参数类型）
#include "CDataManager.h"         // G_CDataManager、find_item、CItem
#include "CInventory.h"           // INVEN_TYPE / eItemDelReason / eItemAddReason、GetInvenTypeFromItemSpace
#include "LogManager.h"           // LogManager::logFormat
#include "CompoundEmblemDeps.h"   // LineFunc(int, const char*, int, unsigned)
#include <vector>

// ============================================================================
// check_error —— ORIG 0x081d3bbc
// 参数：(slotA, itemIdx, slotB, invA, invB, cube)
// 校验：
//   cube == 0                     → 0x11
//   invA.m_addInfo == 0 || != itemIdx → 0x11
//   invB.m_addInfo == 0           → 0x11
//   !cube->is_stackable()         → 0x11
//   cube->GetItemType() != 0x1d   → 0x11
//   !cube->is_included_material_for_limit_cube(itemIdx) → 0x11
//   否则 0
// slotA/slotB 仅参与签名（check_error 内部未使用，见汇编 0x081d3bbc..0x081d3c61）。
// ============================================================================
// [b28c-31] N7：虚调用 callee ORIG %edx vs 4.4 %eax（0x081d3c2x vptr+0xc 槽），
// 其余 57=57 全等（仅 call 目标重定位显示差）。
#include "Dispatcher_UseLimitCube_impl/check_error.inc"
                                         Inven_Item& invA, Inven_Item& invB,
                                         const CItem* cube)
{
    (void)slotA;
    (void)slotB;
    if (cube == 0)
    {
        return 0x11;
    }
    if (invA.m_addInfo == 0 || (unsigned int)invA.m_addInfo != itemIdx)
    {
        return 0x11;
    }
    if (invB.m_addInfo == 0)
    {
        return 0x11;
    }
    if (!cube->is_stackable())
    {
        return 0x11;
    }
    if (cube->GetItemType() != 0x1d)
    {
        return 0x11;
    }
    if (!((CStackableItem*)cube)->is_included_material_for_limit_cube(itemIdx))
    {
        return 0x11;
    }
    return 0;
}

// ============================================================================
// read —— ORIG 0x081d3c62
//   get_short(msg+0x0d) / get_int(msg+0x0f) / get_short(msg+0x13)
//   任一失败 → LineFunc 报错并返回该值（dispatch_template 将其作为 handler1 结果）。
// ============================================================================
#include "Dispatcher_UseLimitCube_impl/read.inc"
{
    // 具名字段（MSG_USE_LIMIT_CUBE 布局见 PacketTypes.h + read.md 汇编：
    // get_short@msg+0xd / get_int@msg+0xf / get_short@msg+0x13）。
    // 调用点直接 cast 表达式：ORIG 为 `mov msg; add $N` 传址（无栈溢出），
    // O0 下引用局部变量会多 store/load 破坏 identical，故不引入具名引用。
    if (!packet.get_short(reinterpret_cast<MSG_USE_LIMIT_CUBE*>(&msg)->m_slotA))
    {
        return LineFunc(0x337d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(reinterpret_cast<MSG_USE_LIMIT_CUBE*>(&msg)->m_itemIdx))
    {
        return LineFunc(0x3381,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(reinterpret_cast<MSG_USE_LIMIT_CUBE*>(&msg)->m_slotB))
    {
        return LineFunc(0x3385,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}

// ============================================================================
// send —— ORIG 0x081d4784
//   param.error（+0x04）非 0 → SendCmdErrorPacket(0x153, error)
//   否则构造 PacketGuard：header(1,0x153) | byte(1) | int(param.resultIdx)
//        | short(param.resultCount) | finalize(true) → CUser::Send
// ============================================================================
// [round3-L30 修] ORIG 0x081d4784：error **无具名槽**（P0）——
//   判空 `movzbl 0x4(%eax),%eax; test %al,%al`（寄存器直判），使用时**重新读**
//   `mov -0xc(%ebp),%eax; movzbl 0x4(%eax),%eax; movzbl %al,%eax`（P48 无 CSE，
//   两次独立字段读）⇒ 删我方 `unsigned char error` 具名局部（旧判词 P73/P248 作废）。
#include "Dispatcher_UseLimitCube_impl/send.inc"
{
    stUseLimitCube& st = *reinterpret_cast<stUseLimitCube*>(&param);
    if (st.m_4 != 0)                     // +0x04（ORIG movzbl + test %al 直判）
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x153, st.m_4);
        return;
    }
    int resultIdx = st.m_8;              // +0x08 movl → put_int
    int resultCount = st.m_c;            // +0x0c movl → put_short（截断 cwtl）
    PacketGuard packet;
    packet.put_header(1, 0x153);
    packet.put_byte(1);
    packet.put_int(resultIdx);
    packet.put_short(resultCount);
    packet.finalize(true);
    user->Send(packet);
}

// ============================================================================
// process —— ORIG 0x081d3d38（handler2：dispatch_template 校验后调用）
// ============================================================================
// [b28d-28] DIFF | ORIG 0x081d3d38 | P167 MISSING 3×deEv/3×ppEv + 3×pair ctor
// 变体 + getCurCharacInvenRefR（迭代器重载 + 取值路径缺口）+ N82 帧
// （0xcac0 vs 0xcaf0）；未闭合缺口。
#include "Dispatcher_UseLimitCube_impl/process.inc"
{
    if (user->get_state() != 3)
    {
        return 0;
    }

    stUseLimitCube& st = *reinterpret_cast<stUseLimitCube*>(&param);
    MSG_USE_LIMIT_CUBE& m = *reinterpret_cast<MSG_USE_LIMIT_CUBE*>(&msg);

    // ORIG movzwl+cwtl（process.md 0x39/0x53）＝ short 读后符号扩展入 int。
    int slotA = m.m_slotA;
    unsigned int itemIdx = m.m_itemIdx;
    int slotB = m.m_slotB;

    // ---- 槽物品读取（INVEN_TYPE_EQUIP=1）----
    Inven_Item invA = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1, slotA);
    Inven_Item invB = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1, slotB);

    // cube = find_item(invB.m_addInfo)，source = find_item(invA.m_addInfo)
    CItem* cube = G_CDataManager()->find_item(invB.m_addInfo);
    CItem* source = G_CDataManager()->find_item(invA.m_addInfo);

    // ---- 物品锁检查 ----
    int lockSpace = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)0);
    if (user->CheckItemLock(lockSpace, slotA))
    {
        st.m_4 = 0x1;
        return 0;
    }

    // ---- 校验 ----
    int err = check_error(slotA, itemIdx, slotB, invA, invB, cube);
    if (err != 0)
    {
        st.m_4 = (unsigned char)err;
        return 0;
    }

    // ---- 结果物品（pair<int,int>）----
    std::pair<int, int> limitPair;
    ((CStackableItem*)cube)->get_limit_cube_item(limitPair);
    if (limitPair.first == 0 && limitPair.second == 0)
    {
        st.m_4 = 0x11;
        return 0;
    }
    st.m_8 = limitPair.first;
    st.m_c = limitPair.second;

    // ---- 材料计数 ----
    int count1 = ((CStackableItem*)cube)->get_1st_material_count(itemIdx);
    int mat2Idx = ((CStackableItem*)cube)->get_2nd_material_index();
    int mat2Count = ((CStackableItem*)cube)->get_2nd_material_count();

    // ---- 材料表 ----
    std::vector<std::pair<int, int> > materials;
    materials.push_back(std::make_pair((int)itemIdx, count1));
    materials.push_back(std::make_pair(invB.m_addInfo, 1));
    materials.push_back(std::make_pair(mat2Idx, mat2Count));

    if (user->getCurCharacInvenR()->CheckNeedItemList(materials) == 0)
    {
        st.m_4 = 0x11;
        return 0;
    }

    // ---- 临时库存模拟删除（tryDeleteEventItems）----
    {
        CInventory tempInv;
        InventoryMemory tempMem;
        tempInv.SetInventoryMemory(&tempMem);
        tempInv.setCopy(*user->getCurCharacInvenR());
        if (tempInv.tryDeleteEventItems(materials) == 0)
        {
            st.m_4 = 0x11;
            return 0;
        }
    }

    // ---- 源信息复制（source 非 stackable 时）----
    // ORIG：local_35 = invA（整体 Inven_Item 拷贝，含 m_amp/m_attr/m_addInfo2/m_fieldd）；
    //        随后仅 SetReSealCount(invA.GetReSealCount()&0xff) + set_add_info(invA.get_add_info())。
    Inven_Item out;
    out.reset();
    unsigned char resealCount = 0;
    int copyAddInfo = 0;
    if (!source->is_stackable())
    {
        out = invA;
        resealCount = invA.GetReSealCount();
        copyAddInfo = invA.get_add_info();
    }

    // ---- 三次删除 + 更新元组 ----
    std::vector<int> updateSlots;  // 每 3 个 int 一组 {0, slot, count}

    // ① source（材料）
    if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, slotA, count1,
                                                (eItemDelReason)8, true) != 0)
    {
        updateSlots.push_back(0);
        updateSlots.push_back(slotA);
        updateSlots.push_back(count1);
    }
    // ② cube（立方体）
    if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, slotB, 1,
                                                (eItemDelReason)8, true) != 0)
    {
        updateSlots.push_back(0);
        updateSlots.push_back(slotB);
        updateSlots.push_back(1);
    }
    // ③ 第二材料
    int mat2Slot = user->getCurCharacInvenR()->check_item_exist(mat2Idx);
    if (mat2Slot < 0)
    {
        st.m_4 = 0x11;
        return 0;
    }
    if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, mat2Slot, mat2Count,
                                                (eItemDelReason)8, true) != 0)
    {
        updateSlots.push_back(0);
        updateSlots.push_back(mat2Slot);
        updateSlots.push_back(mat2Count);
    }

    // ---- 对已删除槽逐组 SendUpdateItem ----
    // [轴B 回滚·N] 表内行 jg/setb(81d4433) 为**假阳性**：ORIG 该点是
    // `cmpl $0x9,-0x20(%ebp); jg`（迭代器循环的计数上限），与我方此处的
    // `size_t i + 2 < updateSlots.size()` 循环尾测**上下文不同构**。
    // 试改 int 后符号性虽对齐（setb→setl），但 matched 144→144、
    // gap −96 不变、TU 四数 131/0/3/34 不变 ⇒ 无收益，按判据回滚。
    for (size_t i = 0; i + 2 < updateSlots.size(); i += 3)
    {
        int slot = updateSlots[i + 1];
        user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
    }

    // ---- 结果物品生成 ----
    Inven_Item resultItem;
    resultItem.reset();
    CItem* resultDef = G_CDataManager()->find_item(limitPair.first);  // 结果对象索引
    if (resultDef == 0)
    {
        st.m_4 = 0x11;
        return 0;
    }
    resultDef->make_item(resultItem);
    if (!resultDef->is_stackable())
    {
        resultItem = out;
        resultItem.SetReSealCount(resealCount);
        resultItem.set_add_info(copyAddInfo);
    }
    else
    {
        resultItem.m_fieldd = limitPair.second;
    }

    // ---- 插入背包 ----
    int newSlot = user->getCurCharacInvenW()->insertItemIntoInventory(
        resultItem, (eItemAddReason)9, true, true);
    if (newSlot < 0)
    {
        st.m_4 = 0x11;
        return 0;
    }
    st.m_4 = 0x0;

    // ---- wrong-result 日志 ----
    int cubeIdx = invB.m_addInfo;
    int resultIdx = limitPair.first;
    if (cubeIdx == 0x1f06)
    {
        if (resultIdx <= 0x671d || resultIdx > 0x6749)
        {
            LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x344b,
                                  "LIMIT CUBE : WRONG RESULT - cube_item(%u), result_item(%u)",
                                  (unsigned int)cubeIdx, (unsigned int)resultIdx);
        }
    }
    else if (cubeIdx == 0x1f11)
    {
        if (resultIdx <= 0x6749 || resultIdx > 0x6775)
        {
            LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x3452,
                                  "LIMIT CUBE : WRONG RESULT - cube_item(%u), result_item(%u)",
                                  (unsigned int)cubeIdx, (unsigned int)resultIdx);
        }
    }

    // ---- 更新结果槽 ----
    user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, newSlot);

    return 0;
}
