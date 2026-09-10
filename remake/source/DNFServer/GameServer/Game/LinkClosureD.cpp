// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 LinkClosureD/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
#include "LinkClosureD/_writeIncorrectItemInventoryForRecovery.inc"
#include "LinkClosureD/meterial_item_change.inc"
#include "LinkClosureD/cago_meterial_item_change.inc"
#include "LinkClosureD/_verifyInventory.inc"
}
#include "CItem.h"
#include "CDataManager.h"             // G_CDataManager / find_item
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString
#include "CMailBoxHelper.h"           // ReqDBSendNewSystemMail
#include "GlobalData.h"               // s_systemTime_
#include "CSystemTime.h"

// ---------------------------------------------------------------------------
// ORIG T 0x08678166 sz 0x4e（报告全文已读）：ctx 首槽为 InterfacePacketBuf*，
// put_short(dungeonIdx) → put_byte(diff) → 返回 1。
// 消费点：CUser::SendDungeonClearList 经 enumDungeonClear 回调（CUser.h 已修正拼写）。
// ---------------------------------------------------------------------------
// [b28d-solo-LinkClosureD] DIFF→N 封顶 | N7+P75（ORIG 0x08678166）：4.4 对
// `userData += 4` 恒「解引用在前 + addl mem 在后」，4.1 为 lea 前置写回+旧值
// 解引用（后缀 *p++ 发射形）；引用形 `InterfacePacketBuf**& p` 引入引用槽
// 24→29 insn 反劣；movswl/movsbl 目标 %eax vs ORIG %edx（N7，本 TU 未路由
// 4.4.6，P230 限定）。语义等价（先取旧值再按 4 字节前移）。
#include "LinkClosureD/fnEnumDungeonClear.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x084bfb84 sz 0x1a4（报告缺失；objdump 0x084bfb84..0x084bfd28 逐指令
// 转写，下称证据 A）。语义：时间窗检查通过后按 itemIdx 生成物品附 add_info=1，
// RDAR 脚本文案作标题/正文经 ReqDBSendNewSystemMail 系统邮件投递。分支细节：
//   now = GlobalData::s_systemTime_.getCurSec()（证据 A 84bfb8f）
//   end > now → 返回；start3 != 0 且 start3 < now → 返回；begin > end → 返回
//   Inven_Item item;（默认 ctor 0x080cb854）
//   CItem* found = G_CDataManager()->find_item(itemIdx)；空 → 返回
//   itemIdx 存 item+0x02 槽（-0x64，find_item 前预存）；found->make_item(item)
//   （vptr+0x08 槽）→ item.set_add_info(1)
//   title = g_scriptStringManager_.findString(4, titleStr, 0)；strncpy(t,0x14)
//   msg   = g_scriptStringManager_.findString(4, msgStr, 0)；strncpy(m,0xff)
//   ReqDBSendNewSystemMail(title, item, 0, characNo, msg, strlen(msg),
//                          p8, group, false, false)
// 参数表（证据 A 槽序）：0x8 user / 0xc begin / 0x10 end / 0x14 start3 /
//   0x18 itemIdx / 0x1c titleStr / 0x20 msgStr / 0x24 p8(uint)。
// ---------------------------------------------------------------------------
// [b28d-solo-LinkClosureD] NEAR | 真语义 bug 修正：ORIG 0x084bfbfb
// `mov 0x18(%ebp),%eax; mov %eax,-0x64(%ebp)` = make_item 前写 item.m_addInfo
// （原译漏写）。另修：nAddInfo 具名 int（ORIG 0x084bfbc8）、body[0x100]（ORIG
// 栈距 0x100，Ghidra local_16a[256]）、P227 内联实参（group→%ebx/strlen→%esi/
// characNo→%eax 无槽）。帧 0x190 与全部具名槽对齐；残余 = title/item/body 槽
// 整体浅 1 字节（4.1 在 title 与 now 间留 1B 空洞，N95/P28 族）。
#include "LinkClosureD/RewardItem.inc"
}

// ---------------------------------------------------------------------------
// meterial_item_change(int, SIG_LOAD_INVENTORY*, Inven_Item*)
// ORIG T _Z20meterial_item_changeiP18SIG_LOAD_INVENTORYP10Inven_Item @0x084c3da5
// sz0x187。报告 docs/class_func_reports/_lt_global_gt_/meterial_item_change.md。
// sig/item 空判 → logFormat；ExceptionItem::find_index(item->m_addInfo) 且
// m_field1==3 命中 → m_field1=0xa；do-while i=0xc9..0xf8 扫 SIG 材料槽
// （0x5949 + i*0x3d，步长 0x3d）：空槽 setCopy+reset 返回；同 addInfo 槽
// m_addInfo2 累加后 reset 返回；溢出 logFormat 0x161d。
// ---------------------------------------------------------------------------
// [b28d-solo-LinkClosureD] NEAR | 已收：P183 去 dst 缓存（ORIG 每点重发
// imul 链，0x084c3e40/3e5e/3ed7/3efb 四组）+ P214① 正极性 if(A&&B) 包裹
// （0x084c3e11..e2e 短路物化 mov$1/mov$0 + test 无槽）+ P283 goto 固定物理
// 块序（入口 jmp→非空处理→++idx/尾测 jle 直跳→overflow→体头 isEmpty
// xor$1;jne→空处理）。残余 = ORIG `add $0x5950` vs 我们 `add $0x5940;
// mov 0x10(%eax)`（N4/P30 常量折叠）+ 2 条对齐 nop（§6.1）。
#include "LinkClosureD/meterial_item_change_1.inc"
}

// ---------------------------------------------------------------------------
// cago_meterial_item_change(int, SIG_LOAD_INVENTORY*, Inven_Item*)
// ORIG T _Z25cago_meterial_item_changeiP18SIG_LOAD_INVENTORYP10Inven_Item
// @0x084c3f2c sz0x66。报告 _lt_global_gt_/cago_meterial_item_change.md：
// cargo 版仅做 find_index && m_field1==3 命中后的 m_field1=0xa 标记。
// ---------------------------------------------------------------------------
#include "LinkClosureD/cago_meterial_item_change_1.inc"
}

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

// ---------------------------------------------------------------------------
// _verifyItem(int slot, const Inven_Item* item, INVENTORY_TYPE type,
//             bool b1, bool b2, CUser* user)
// ORIG W _Z11_verifyItemiPK10Inven_Item14INVENTORY_TYPEbbP5CUser @0x084efcdd
// sz0x398。报告缺失（objdump 0x084efcdd..0x084f0075 逐段转写）。返回错误码，
// 0 = 通过；0x1562..0x15f8 为各校验失败码（语义见各分支注释）。
// 判例（ADeepY4[7]）：DIFF(N封)，gap +9（orig 290 / our 299，was 301）。真 bug 修复：
//   m_field0 校验 `if(m_field0 && !b1)`→`if(m_field0 && b1)`。ORIG 84efe89 区：
//   `movzbl(%item)(m_field0);test;je续;cmpb $0,-0x4c(b1);je续;mov $0x157d`——失败条件是
//   m_field0 非零 **且** b1 非零（P196 布尔反演，equip 段 b1=true 拒带残留 flag 物品、
//   inven 段 b1=false 放行）；原译写 !b1 为反。翻转后 xor $1 消失、代码 301→299。
//   残余：GUID 分支 ORIG 将 guid/characNo/accId 入 edi/esi/ebx 提前(cMyTrace ctor 前)，
//   4.4 下栈槽(N82)；stackable 位测 ORIG `shl %cl;and $0x69c`、我们强度归约 `shr;and $1`
//   (N86b 等价)；非 stackable type 分发 ORIG 链式 cmp、我们区间式。错误码 0x1562..0x15f8
//   与空槽全零/rarity/attachtype 各分支语义全部对应。
// ---------------------------------------------------------------------------
// [b28d-solo-LinkClosureD] DIFF→N 封顶 | 已收：P227 全表达式临时链消 GUID
// 段具名局部（guid→%edi/characNo→%esi/accId→%ebx 跨 ctor 保活，帧 0x80→0x6c
// 与全部槽 -0x1c/-0x20/-0x30/-0x45/-0x4c/-0x50/-0x54 对齐）、def 直用槽消
// stackDef、switch 化 type 分发（二分树 jg/jge/je 全等）、register t（P231）。
// 残余 N 类：①位测 `(1<<t)&0x69c`/`(1<<at)&0x27` 被 4.4 强度归约 shr/and $1
// （ORIG shl/and，N6/N86b，P231 已试）；②`get_rarity()<=1` ORIG `add $0x0;ja`
// 我们 `jg`（返回类型宽度/setcc-fold）；③空槽 `return 0` 独立块 vs ORIG je 直
// 达 epilogue（P47/P59 块布局）；④GUID 实参 ebx 前置（P52 变体）。
#include "LinkClosureD/_verifyItem_1.inc"
}

// ---------------------------------------------------------------------------
// _writeIncorrectItemInventoryForRecovery(uint, const Inven_Item&, const char*,
//                                        int, int)
// ORIG W _Z39_writeIncorrectItemInventoryForRecoveryjRK10Inven_ItemPKcii
// @0x084f0075 sz0xd0。报告缺失（objdump 转写）：单条 ERROR_ITEM_LOG 日志，
// 字段序（含 m_amp 首 4 字节 int 视图）见 logFormat 实参。
// ---------------------------------------------------------------------------
// [b28d-solo-LinkClosureD] AE | 实参槽序按 ORIG 0x084f00b9..010d 修正为
// characNo,field0,field1,addInfo,GetItemAttr,addInfo2(+0x7),fieldb(+0xb),
// fieldd(+0xd),amp 首 int(+0x11),tag,slot,err；amp 内联表达式消具名槽（P0）后
// 帧 0x7c→0x6c 与 ORIG 全等，全部槽/寄存器分配逐条一致（残余仅 .o 重定位占位，
// compare_tu AE 归一化后 clean）。
#include "LinkClosureD/_writeIncorrectItemInventoryForRecovery_1.inc"
}
