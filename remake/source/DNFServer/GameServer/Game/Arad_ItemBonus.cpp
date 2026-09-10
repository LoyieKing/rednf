// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Arad_ItemBonus/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— ARAD 物品/奖励域实现（W6_ARAD_ItemBonus 批次独占 TU）。
//
// 符号清单（nm df_game_r 核实，全部本域产出）：
//   ARAD::CheckAccountItem(CUser*, uint, PurchaseLimitItem::Info&) T @0x08196ac8
//   ARAD::send_mail(ARAD_DATAS::AradMailInfo&)                     T @0x08196b03
//   ARAD::importCashShopPurcahseBonusItemList(char*, map&)         T @0x088b62ec
//   ARAD::ARAD_ITEM::arad_process_item(CUser*, Inven_Item const&)  T @0x081973fa
//   ARAD::ARAD_ITEM::arad_secret_shop_event_buff(CUser*, ulong)    T @0x08196dd3
//   ARAD::ARAD_ITEM::arad_secret_shop_event_item(CUser*, ulong, CStackableItem*) T @0x08196ed7
//   ARAD::CeraShopPurcahseCountBonus::{LoadScript 0x081abf14,
//     GiveBonusItem 0x081abf44, CheckBonusItem 0x081abff0,
//     _findBonusItem 0x081ac04c, _processBonusItem 0x081ac13e}     T
//   PurcahseBonus ctor / operator==、ARAD_DATAS MailInfo/ItemInfo ctor、
//   GetItemAttr 为 ORIG W 弱符号 → inline 定义于 Arad_ItemBonus.h /
//   ItemVendingMachine.h（与 ORIG 相同 TU 归属）。
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
const char kProcessPretty[] =
    "void ARAD::CeraShopPurcahseCountBonus::_processBonusItem(CUser*, int, "
    "const ARAD::PurcahseBonus&)";
const char kAradFuncFile[] = "localjapan/Arad_Function_Set.cpp";

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

// ---------------------------------------------------------------------------
// CheckAccountItem —— ORIG 0x08196ac8（docs/class_func_reports/ARAD/
// CheckAccountItem.md）：单例 PurchaseLimitItem 查询包装，命中→false，
// 未命中→true。
// ---------------------------------------------------------------------------
#include "Arad_ItemBonus/CheckAccountItem.inc"
}

// ---------------------------------------------------------------------------
// importCashShopPurcahseBonusItemList —— ORIG 0x088b62ec
// （docs/class_func_reports/ARAD/importCashShopPurcahseBonusItemList.md）。
// 错误码口径与汇编一致；EOF 路径 ORIG `mov $0,%ebx`（+0x34c）⇒ 正常结束返回 0。
// ---------------------------------------------------------------------------
// b28d-solo-Arad_ItemBonus: ORIG 无 result/again 局部（P0：错误码经 %ebx 直返、
// 循环条件仅 ScanType；%esi 为 EH 清理用的返回标志）；`int key = 0;` 先于
// bonus/list 构造、`key = ScanInt(&ok);` 独立赋值（ORIG +0x76/+0x93）。
#include "Arad_ItemBonus/importCashShopPurcahseBonusItemList.inc"
}

// ---------------------------------------------------------------------------
// CeraShopPurcahseCountBonus::LoadScript —— ORIG 0x081abf14
// （docs/class_func_reports/ARAD__CeraShopPurcahseCountBonus/LoadScript.md）：
// return import(...) < 1 即失败返回 true。
// ---------------------------------------------------------------------------
#include "Arad_ItemBonus/LoadScript.inc"
}

// ---------------------------------------------------------------------------
// CheckBonusItem —— ORIG 0x081abff0：map.find(key) != end()。
// ---------------------------------------------------------------------------
#include "Arad_ItemBonus/CheckBonusItem.inc"
}

// ---------------------------------------------------------------------------
// GiveBonusItem —— ORIG 0x081abf44（GiveBonusItem.md）：
// keyCheck(goodsIdx) 命中后 find(key)->second 上做阈值匹配再发放；
// itemIdx 形参在 ORIG 中未使用，保持签名一致。
// ---------------------------------------------------------------------------
#include "Arad_ItemBonus/GiveBonusItem.inc"
}

// ---------------------------------------------------------------------------
// _findBonusItem —— ORIG 0x081ac04c：遍历 list 取最后一个 m_flag <= threshold
// 的元素 memcpy 0xa 字节到 out；无命中返回 false。
// ---------------------------------------------------------------------------
// b28d-solo-Arad_ItemBonus: ORIG best 为**迭代器**（best=list.end()，槽 -0x18），
// it 槽 -0x14；命中 `best=it`（无 operator*），尾判 `best==list.end()`（-0xc）后
// `out=*best`（结构赋值 ⇒ 0xa 字节 memcpy + 自拷贝守卫，ORIG 0x081ac0d3..0x081ac136）。
#include "Arad_ItemBonus/_findBonusItem.inc"
}

// ---------------------------------------------------------------------------
// _processBonusItem —— ORIG 0x081ac13e（_processBonusItem.md 全文转写）：
//   type==1 代币；type==5 里程券；其它按 itemIdx 发放物品。
// ---------------------------------------------------------------------------
// b28d-solo-Arad_ItemBonus: verdict=IDENTICAL_AE 332=332，p167 42=42 CLEAR。
// 关键旋钮：switch(m_field4) 三路 cmp/je/jmp（P17/P46/P251）；P0 无 oldPay/
// amount/curPay/mileageF 具名局部（各使用点重算，x87 序 price*(amount/1000.0f)）；
// P212 全头部声明复现槽深序（goods -0x30→msg -0x1c）；P221 cMyTrace 全表达式
// 临时链翻 sibling 槽方向；P92 尾日志读 inv 字段；P315/P254 m_field4 改 unsigned
// 翻 find_item 实参两段装载。ORIG 0x081ac13e。
#include "Arad_ItemBonus/_processBonusItem.inc"
}

// ---------------------------------------------------------------------------
// CheckAccountItem 依赖的 PurchaseLimitItem 查询说明见头注释。
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// send_mail —— ORIG 0x08196b03（send_mail.md 全文转写）：邮件流打包入 DB 队列。
// ---------------------------------------------------------------------------
// b28d-solo-Arad_ItemBonus: 依 ORIG 0x08196b03 段逐条复形——P259 Acquire 内联进
// CStreamGuard ctor（无 stream 具名局部）；item 指针具名局部（-0x10）；strlen 结果
// 具名 len（-0xc）再入 strncpy 实参；m_field15f 4 字节 store。
#include "Arad_ItemBonus/send_mail.inc"
}

// ---------------------------------------------------------------------------
// ARAD_ITEM::arad_secret_shop_event_buff —— ORIG 0x08196dd3（报告全文转写）。
// ---------------------------------------------------------------------------
// b28d-solo-Arad_ItemBonus: P0/P52 — ORIG 无 area/vill 具名局部（get_area 结果
// 留 %ebx、getCurCharacVill 留 %esi 跨 G_GameWorld() 保活），实参内联单语句。
#include "Arad_ItemBonus/arad_secret_shop_event_buff.inc"
}

// ---------------------------------------------------------------------------
// ARAD_ITEM::arad_secret_shop_event_item —— ORIG 0x08196ed7（报告全文转写）。
// ---------------------------------------------------------------------------
// b28d-solo-Arad_ItemBonus: P0 — ORIG 无 rabbits 具名引用（每使用点重算
// `stackable->m_arad148.m_vec20`）；box/it 均在循环外默认构造（+0x15a/+0x165），
// 循环内 `box = *it;`；lastSlot 分支 ORIG `setne;test;je <mail>`（P225 形）。
#include "Arad_ItemBonus/arad_secret_shop_event_item.inc"
}

// ---------------------------------------------------------------------------
// ARAD_ITEM::arad_process_item —— ORIG 0x081973fa（arad_process_item.md 转写）。
// ---------------------------------------------------------------------------
// b28d-solo-Arad_ItemBonus: 依 ORIG 0x0819741c..0x081974de 两枚 int 局部——
// restrictCode=-1（-0x18：尾判 ==-1 与 SendUpdateItemList 实参来源）与 code=0
// （-0x1c：&code 作 ProcessSpecialItem 第 4 参出参）；ORIG 未把 code 写回
// restrictCode（原码如此，邮件路径恒走）。P168a①：`if (!call())` 直产
// call;xor$1;test;je 且 then 块内联、log 段外置（ORIG +0x194）。
#include "Arad_ItemBonus/arad_process_item.inc"
}

}  // namespace ARAD
