// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Arad_ItemBonus/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [S5AradDisp 2026-09-11 负结论 N·STL 实例化 EH] 本 TU 残差其一
//   `_ZNSt20__uninitialized_copyILb0EE18uninitialized_copyIN9__gnu_cxx17__normal_iterator
//    IPKN4ARAD13PurcahseBonusESt6vectorIS5_SaIS5_EEEEPS5_EET0_T_SE_SD_`（DIFF 57/40）:
//   ORIG 0x088b7a35..0x088b7adb 保留 catch(...) 岛 —— 0x088b7aac `mov %eax,(%esp);
//   call __cxa_begin_catch` → 0x088b7ac1 `call _ZSt8_DestroyIPN4ARAD13PurcahseBonusEEvT_S3_`
//   → 0x088b7ac6 `call __cxa_rethrow` → 0x088b7acb/0x088b7adf `__cxa_end_catch` +
//   `_Unwind_Resume`（push %esi + 帧 0x20）；我方同符号 size 0x75（无 catch 岛、push %ebx、
//   帧 0x24）。函数体来自 4.4.7 头 `bits/stl_uninitialized.h` 的 __uninitialized_copy<false>
//   （预处理实证含 __try/__catch，故非源码/头问题）：同旗标同头集的 /tmp/probe_pb.cpp
//   探针（ARAD::PurcahseBonus 同形类型 + vector 拷贝）产出 0x8d **保留** catch，而本 TU 的
//   同名实例被 4.4 判「try 区不抛」删除 ⇒ 属 TU 环境/编译器取舍，源码级不可控，且
//   PurcahseBonus 定义在 ItemVendingMachine.h（越界）。仅 .o 级 EH cleanup 差异，
//   对象语义等价 ⇒ 维持 N。
// ============================================================================

#include <math.h>
#include <string.h>

#include <algorithm>
#include <utility>

#include "Arad_ItemBonus.h"

#include "Arad_DataManager.h"          // Singleton<Arad_DataManager>/getPurchaseLimitItem
#include "CMailBoxHelper.h"            // WongWork::CMailBoxHelper::ReqDBSendNewMail
#include "CDataManager.h"
#include "CCeraShop.h"
#include "CInventory.h"
#include "CItemList.h"
#include "CUserCharacInfo.h"
#include "GameTypes.h"                 // cMyTrace / Inven_Item 完整定义链
#include "GlobalData.h"
#include "Inven_Item.h"
#include "MsgQueueMgr.h"
#include "PacketGuard.h"
#include "RDARScriptStringManager.h"
#include "GameWorld.h"                 // G_GameWorld / send_to_area

#include "DNFLexWrapper.h"             // ScanType / ScanInt
#include "NpcScript.h"                 // loadRDARScriptFile
#include "CUser.h"
#include "CSystemTime.h"

namespace {
// [AnonLane2 2026-09-11 匿名轴] 旧 TU 级 `kAradFuncFile` 常量已删（ORIG 全库对
// 该名零命中；ORIG 侧仅匿名 .rodata 串），2 处使用点内联同值字面量。

// ---------------------------------------------------------------------------
// send_mail 写入目标：SIG_MAILBOX_SEND_NEW_MAIL（SigTypes.h 目前为
// m_data[0x168] pad）的字段化镜像。
// 【ABI 镜像】按 ORIG send_mail 反汇编写入偏移逐字段命名，pack(1)，
// sizeof=0x160 ≤ SigTypes.h 实际尺寸 0x168。字段语义见各注释。
// ---------------------------------------------------------------------------
#pragma pack(push, 1)
struct SigMailboxSendNewMailMirror
{
    char m_name[0x1d];        // +0x000 收件人名（strncpy 不保证 NUL）
    unsigned char m_pad1d;    // +0x01d
    int m_field1e;            // +0x01e ← mail.m_field10
    int m_field22;            // +0x022 ← mail.m_fieldC
    int m_field26;            // +0x026 ← mail.m_field4
    unsigned char m_field2a;  // +0x02a ← mail.item.m_flag
    int m_field2b;            // +0x02b ← mail.item.m_fieldC
    int m_field2f;            // +0x02f ← mail.item.m_field10
    unsigned char m_field33;  // +0x033 ← mail.item.GetItemAttr()
    unsigned short m_field34; // +0x034 ← mail.item.m_fieldA
    int m_field36;            // +0x036 ← mail.item.m_field10
    unsigned char m_pad3a[0xa];
    unsigned char m_field44;  // +0x044 置 0
    unsigned short m_field45; // +0x045 置 0
    unsigned char m_pad47[0xf];
    unsigned char m_field56;  // +0x056 置 0
    unsigned char m_pad57;
    unsigned char m_field58;  // +0x058 置 0
    unsigned short m_field59; // +0x059 ← mail.m_field14（short）
    char m_content[0x100];    // +0x05b memset 0 后 strncpy content
    int m_field15b;           // +0x15b ← mail.m_field8（同型直存，ORIG 0x08196c1a）
    // ORIG 0x08196c2b `movzbl (%eax),%eax; movzbl %al,%edx; mov %edx,0x15f(%eax)`
    // ⇒ 目标为 4 字节（uchar→uint 双段零扩展后 32 位 store）。
    unsigned int m_field15f;  // +0x15f ← mail.m_isSystemMail
    unsigned char m_pad163[5];// +0x163（凑 SIG_MAILBOX_SEND_NEW_MAIL 0x168）
};
#pragma pack(pop)

static_assert(sizeof(SigMailboxSendNewMailMirror) == 0x168,
              "SIG_MAILBOX_SEND_NEW_MAIL mirror size");

}  // namespace

namespace ARAD_DATAS
{
}  // namespace ARAD_DATAS

namespace ARAD
{

#include "Arad_ItemBonus/CheckAccountItem.inc"
                      class PurchaseLimitItem::Info& out)
{
    (void)user;  // ORIG 不使用 user 形参（Ghidra/汇编均无引用）
    // P0：ORIG 无 mgr 局部（Get→getPurchaseLimitItem→find 寄存器直传）；
    // 尾部 choose 岛（test;je;mov$0;jmp;mov$1）＝ P56 if-return 形。
    if (Singleton<Arad_DataManager>::Get()->getPurchaseLimitItem()->find(itemIndex, out))
        return false;
    return true;
}

// [SplitC 2026-09-11 batch4 · TU 迁移] importCashShopPurcahseBonusItemList 已迁出至 RDARScriptCeraShop.cpp（ORIG GI 键对齐）。

// [SplitC 2026-09-11 batch2 · TU 迁移] LoadScript 已迁出至 CeraShopPurcahseCountBonus.cpp（ORIG GI 键对齐）。



#include "Arad_ItemBonus/CheckBonusItem.inc"


#include "Arad_ItemBonus/GiveBonusItem.inc"


#include "Arad_ItemBonus/_findBonusItem.inc"


#include "Arad_ItemBonus/_processBonusItem.inc"


// ---------------------------------------------------------------------------
// CheckAccountItem 依赖的 PurchaseLimitItem 查询说明见头注释。
// ---------------------------------------------------------------------------

#include "Arad_ItemBonus/send_mail.inc"


#include "Arad_ItemBonus/arad_secret_shop_event_buff.inc"


#include "Arad_ItemBonus/arad_secret_shop_event_item.inc"
                                            CStackableItem* stackable)
{
    typedef std::vector<CRabbitBox> RabbitVec;

    if (stackable->m_vec20.empty())
        return;
    if (stackable->get_index() == 0x28be4b)
        return;

    if (stackable->get_index() == 0x28be6a)
    {
        // 纯事件广播分支（不发放物品）。
        PacketGuard pg;
        pg.clear();
        pg.put_header(0, 0x100);
        pg.put_byte(1);
        pg.put_int(param);
        pg.put_int(0x708);
        pg.finalize(true);
        user->Send(pg);

        G_GameWorld()->send_to_area(user->getCurCharacVill(),
                                    user->get_area(false), pg);
        return;
    }

    int lastSlot = -1;
    // P74/P50：box 与 it 均默认构造（ORIG +0x15a/+0x165），循环内赋值。
    CRabbitBox box;
    RabbitVec::iterator it;
    it = stackable->m_vec20.begin();
    while (it != stackable->m_vec20.end())
    {
        box = *it;
        // P0/P48：ORIG 无 rabbitIdx 局部，getItemIndex() 在使用点各调一次
        // （+0x1bc find_item 实参 / +0x1f5 inv.m_addInfo）。
        CItem* item = G_CDataManager()->find_item(box.getItemIndex());
        if (item != 0)
        {
            Inven_Item inv;
            // ORIG 序：+0x1e7 C1Ev → +0x1ef/+0x203 两字段写 → +0x20b make_item。
            inv.m_addInfo = box.getItemIndex();            // +0x02
            inv.m_addInfo2 = box.getItemCount();           // +0x07
            item->make_item(inv);
            lastSlot = user->getCurCharacInvenW()->insertItemIntoInventory(
                inv, (eItemAddReason)0xd, true, false);   // ORIG reason 0xd
            // P225：ORIG `cmpl $-1;setne;test;je <mail>`（update 直落、mail 外置）
            // ⇒ while+goto 形；label 置于 ++it 前（ORIG +0x348 nop/+0x349 ++it）。
            while (lastSlot != -1)
            {
                if (!user->SendUpdateItemList(CUser::eSendTarget_1,
                                              (ENUM_ITEMSPACE)0, lastSlot))
                    return;                            // LAB_0819735a 提前结束
                goto slot_done;
            }
            WongWork::CMailBoxHelper::ReqDBSendNewMail(
                user, inv, 0, user->getCurCharacNo(), true, 0, 0);
        }
    slot_done:
        ++it;
    }

    PacketGuard pg;
    pg.clear();
    pg.put_header(0, 0x100);
    pg.put_byte(1);
    pg.put_int(param);
    pg.put_int(0x708);
    pg.finalize(true);

    G_GameWorld()->send_to_area(user->getCurCharacVill(),
                                user->get_area(false), pg);
}

#include "Arad_ItemBonus/arad_process_item.inc"


}  // namespace ARAD
