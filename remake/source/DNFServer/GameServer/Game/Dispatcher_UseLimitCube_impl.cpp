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
int Dispatcher_UseLimitCube::check_error(int slotA, unsigned int itemIdx, int slotB,
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
int Dispatcher_UseLimitCube::read(PacketBuf& packet, MSG_BASE& msg)
{
    if (!packet.get_short(*(short*)((char*)&msg + 0x0d)))
    {
        return LineFunc(0x337d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*(unsigned int*)((char*)&msg + 0x0f)))
    {
        return LineFunc(0x3381,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*(short*)((char*)&msg + 0x13)))
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
void Dispatcher_UseLimitCube::send(CUser* user, ParamBase& param)
{
    unsigned char error = *(unsigned char*)((char*)&param + 0x04);
    if (error != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x153, error);
        return;
    }
    int resultIdx = *(int*)((char*)&param + 0x08);
    int resultCount = *(int*)((char*)&param + 0x0c);
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
int Dispatcher_UseLimitCube::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    if (user->get_state() != 3)
    {
        return 0;
    }

    stUseLimitCube& st = *reinterpret_cast<stUseLimitCube*>(&param);
    char* pMsg = (char*)&msg;

    int slotA = *(short*)(pMsg + 0x0d);
    unsigned int itemIdx = *(unsigned int*)(pMsg + 0x0f);
    int slotB = *(short*)(pMsg + 0x13);

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
        *(unsigned char*)((char*)&st + 0x04) = 0x1;
        return 0;
    }

    // ---- 校验 ----
    int err = check_error(slotA, itemIdx, slotB, invA, invB, cube);
    if (err != 0)
    {
        *(unsigned char*)((char*)&st + 0x04) = (unsigned char)err;
        return 0;
    }

    // ---- 结果物品（pair<int,int>）----
    std::pair<int, int> limitPair;
    ((CStackableItem*)cube)->get_limit_cube_item(limitPair);
    if (limitPair.first == 0 && limitPair.second == 0)
    {
        *(unsigned char*)((char*)&st + 0x04) = 0x11;
        return 0;
    }
    *(int*)((char*)&st + 0x08) = limitPair.first;
    *(int*)((char*)&st + 0x0c) = limitPair.second;

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
        *(unsigned char*)((char*)&st + 0x04) = 0x11;
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
            *(unsigned char*)((char*)&st + 0x04) = 0x11;
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
        *(unsigned char*)((char*)&st + 0x04) = 0x11;
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
        *(unsigned char*)((char*)&st + 0x04) = 0x11;
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
        *(unsigned char*)((char*)&st + 0x04) = 0x11;
        return 0;
    }
    *(unsigned char*)((char*)&st + 0x04) = 0x0;

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
