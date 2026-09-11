// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LinkClosureD/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// LinkClosureD.cpp — 全量链接收敛批 D 散件 producer（LinkClosureD 批次）
//
// 覆盖符号（ORIG nm -S 核对）：
//   fnEnumDungeonClear(short,char,char*)      T 0x08678166（报告 _lt_global_gt_/fnEnumDungeonClear.md）
//   RewardItem(CUser*,long,long,long,uint,PKc,PKc,uint) T 0x084bfb84（报告缺失，objdump 逐指令转写）
//
#include "SigTypes.h"                 // SIG_LOAD_INVENTORY 权威布局
#include "CTitleBook.h"               // item_lock::stItemLockRef（CItemLockTimer/lock 池权威布局）
#include "CDataManager.h"             // G_CDataManager / find_item
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString
#include "CMailBoxHelper.h"           // ReqDBSendNewSystemMail
#include "GlobalData.h"               // s_systemTime_
#include "CSystemTime.h"
#include "CUserCharacInfo.h"
#include "Inven_Item.h"
#include "InterfacePacketBuf.h"
#include "CItem.h"
#include "CUser.h"
#include "Arad_DataManager.h"     // ARAD::Singleton / getExceptionItem / ExceptionItem
#include "GameTypes.h"            // cMyTrace
#include "ItemGloballyUniqueIdentifierGenerator.h"  // itemGloballyUniqueIdentifier_t
#include "DNFFunctionLib.h"       // DNFFLib::Binary2Hex（GUID 渲染）
#include "CInventory.h"           // GetSlotItemType（_verifyItem type==1 分支）

// ORIG mangled 依赖（仓库无 producer —— §8.1 仅声明，U 引用合规，收敛清单已报 Main）：
//   _Z11_verifyItemiPK10Inven_Item14INVENTORY_TYPEbbP5CUser            W 0x084efcdd
//   _Z39_writeIncorrectItemInventoryForRecoveryjRK10Inven_ItemPKcii    W 0x084f0075
//   _Z20meterial_item_changeiP18SIG_LOAD_INVENTORYP10Inven_Item        T 0x084c3da5
//   _Z25cago_meterial_item_changeiP18SIG_LOAD_INVENTORYP10Inven_Item   T 0x084c3f2c
// INVENTORY_TYPE 为 ORIG mangled 内嵌枚举（14 长度名，值 0..3 实证；成员名
// 未还原 [推断]），由本 TU 建模承载。
enum INVENTORY_TYPE
{
    INVENTORY_TYPE_0 = 0,
    INVENTORY_TYPE_1 = 1,
    INVENTORY_TYPE_2 = 2,
    INVENTORY_TYPE_3 = 3
};

#include "LinkClosureD/_verifyItem.inc"
                bool b1, bool b2, CUser* user);
#include "LinkClosureD/_writeIncorrectItemInventoryForRecovery.inc"
                                             const Inven_Item& item,
                                             const char* tag, int err,
                                             int slot);
#include "LinkClosureD/meterial_item_change.inc"
#include "LinkClosureD/cago_meterial_item_change.inc"
                               Inven_Item* item);

// ORIG 段内联展开（无独立符号）：collectLock 逐段直接内联，用共享
// 函数级临时 lock/ref 槽对齐 ORIG 的 6 个栈槽（frame 0x38 → disp8 编码）。

#include "LinkClosureD/_verifyInventory.inc"

#include "CItem.h"
#include "CDataManager.h"             // G_CDataManager / find_item
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString
#include "CMailBoxHelper.h"           // ReqDBSendNewSystemMail
#include "GlobalData.h"               // s_systemTime_
#include "CSystemTime.h"

#include "LinkClosureD/fnEnumDungeonClear.inc"


#include "LinkClosureD/RewardItem.inc"


#include "LinkClosureD/meterial_item_change_1.inc"


#include "LinkClosureD/cago_meterial_item_change_1.inc"


// ---------------------------------------------------------------------------
// CItemGloballyUniqueIdentifierPrint（ORIG W 0x84ebe58 族）：ctor memset 0x15、
// op() = Binary2Hex(&guid,0xa,buf)、dtor 空。与 cUserHistoryLog.cpp 局部视图同型。
// ---------------------------------------------------------------------------
class CItemGloballyUniqueIdentifierPrint
{
public:
    CItemGloballyUniqueIdentifierPrint() { memset(m_buf, 0, sizeof(m_buf)); }
    ~CItemGloballyUniqueIdentifierPrint() {}

    const char* operator()(const itemGloballyUniqueIdentifier_t& guid)
    {
        DNFFLib::Binary2Hex(reinterpret_cast<const unsigned char*>(&guid), 0xa,
                            m_buf);
        return m_buf;
    }

private:
    char m_buf[0x15];
};

#include "LinkClosureD/_verifyItem_1.inc"


#include "LinkClosureD/_writeIncorrectItemInventoryForRecovery_1.inc"

