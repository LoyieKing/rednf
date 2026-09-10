// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Store/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "LogManager.h"
// df_game_r Game/ Store 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 对照 docs/class_func_reports/Store.md：
//   GetSellItemPrice(Inven_Item&,...) 0x08618ea0
//   GetSellItemPrice(CUser*,...)      0x08619008
//   repair_equip                      0x08619cec
//   user_buy_item                     0x08618a44
//   user_sell_item                    0x086193f8
#include "CInventory.h"
#include "CDataManager.h"
#include "CItemList.h"
#include "CGameManager.h"
#include "CEnvironment.h"   // GlobalInstance<Store>（ORIG G_Store 调 inst_ptr）
#include "CDungeon.h"
#include "GlobalData.h"
#include "CServerProxyMgr.h"
#include <string.h>
#include <vector>
#include "CHackLog_InvitationPaperSellToNPC.h"   // user_sell_item hack 检查
#include "CHackAnalyzer.h"                        // addServerHackCnt
#include "HistoryLog.h"                           // WriteSellItem

#include "CStatisticServerProxy.h"
#include "Packet_DeathTower_Statistic_Value.h"   // 类带 PacketHeader(0xc3b,0x15) ctor（ORIG C1 0x0860ffc2）

// ---------------------------------------------------------------------------
// user_buy_item(CUser*, int itemId, int count)
// ---------------------------------------------------------------------------
// [b28d-solo-Store] 真语义 bug（ORIG 0x08618b0e 实证）：可堆叠路的类型分派是
// 三次独立**虚调 GetItemType()**（vptr+0xc ⇒ _ZTV10CEquipItem word[5]=
// CEquipItem::GetItemType 0x08514d26），映射 field1=3/10/4；旧实现误用非虚
// GetAttachType()（0x080f12e2）且缓存成具名局部 + else-if 链。
// 另修：insertItemIntoInventory 末两 bool 实参极性（ORIG 0x08618c52 压 (0,1,0)
// ⇒ (item, reason0, true, false)）；getCurCharacInvenR/W 改链式（P183）；
// cMyTrace 改全表达式临时链（P227）；第三实参改直读 m_addInfo2（ORIG -0x6e 直读）。
#include "Store/user_buy_item.inc"
}

// ---- 前向声明 ----
enum DEATHTOWER_VALUE_STATISTIC_FIELD
{
    DEATHTOWER_VALUE_STATISTIC_FIELD_0 = 0,
    DEATHTOWER_VALUE_STATISTIC_FIELD_9 = 9
};

class CDeathTowerValueStatistic
{
public:
    bool SendDeathTowerValueStatistic(DEATHTOWER_VALUE_STATISTIC_FIELD field, char a, char b, unsigned int c);
};

extern CDeathTowerValueStatistic* GetInstanceDeathTowerValueStatistic()
{
    static CDeathTowerValueStatistic instance;
    return &instance;
}

// [b28d-solo-Store] 真语义 bug 修复（ORIG 0x0860f342 实证）：field != 10 时 ORIG 不早退，
// 而是 packet.m_counterIdx = field 后照常发送并 return true（+0xcc 块 `mov 0xc(%ebp),%eax;
// mov %eax,-0x10(%ebp)` 直落共享尾 +0xd2）。旧实现 `return false` 属漏发包。
// P0：ORIG 无独立 f 局部槽（m_counterIdx 即 -0x10 槽，3/4/5/6/field 直写成员）。
#include "Store/SendDeathTowerValueStatistic.inc"
}

// 修理价格系数（由 InitEquipmentScript 从脚本加载）
static float g_repairRate = 0.0f;

#include "Store/getEquipmentRepairPrice.inc"
}

// getItemSellPrice 的唯一实现归 ItemUpgradeVtableB2.cpp（ORIG 0x0898c86e）；
// 本 TU 的 Store::GetSellItemPrice 仅调用该共享自由函数。
#include "Store/getItemSellPrice.inc"
}

// ---------------------------------------------------------------------------
// GetSellItemPrice(Inven_Item&, const CItem*, short, bool, int&)
// ---------------------------------------------------------------------------
// [b28d-solo-Store] 真语义 bug：ORIG 0x08618f79 比较对象是 item.m_fieldb(+0xb)，
// 旧实现误用 count（`if (get_endurance() >= count) item.m_fieldb = ...`）——修满耐久
// 判定完全错位。另修正：`if (!isEquipableItemType())`（bool callee + P292 xor 形）、
// 正极性短路包裹（P214①）、m_itemType 判序 ==0xb 先于 <10、删 cnt 具名局部（P0）。
#include "Store/GetSellItemPrice.inc"
}

// ---------------------------------------------------------------------------
// GetSellItemPrice(CUser*, char invenType, short slot, short count, bool flag,
//                  int& outPrice, int& outCount) —— ORIG 0x08619008
// [b28d-solo-Store] 真语义 bug：ORIG 0x086193b5 写第 6 参 = pItem->get_index()，
// 并把第 7 参直接作 5 参重载的价格出参（0x24(%ebp)→0x14(%esp)）；旧实现写成
// outCount = get_index() 且经局部 price 中转 → 两个引用出参交叉错位。
// 另修：invenType 分派是 switch（ORIG 0x08619037 线性决策树，P251/P46）；
// default 返 0x11（旧写 0x13）；find_item 走 CDataManager::find_item（非 m_itemList）。
// ---------------------------------------------------------------------------
#include "Store/GetSellItemPrice_1.inc"
}

// ---------------------------------------------------------------------------
// user_sell_item(CUser*, char invenType, short slot, short count) —— ORIG 0x86193f8
// ---------------------------------------------------------------------------
// [b28d-solo-Store] 真语义 bug：hack 检查的两个函数是**静态**调用（ORIG 0x08619c43
// `call IsInvitationPaper(int)` 前无 this 压栈、0x08619c4c `call GetHackType()` 亦无
// this）——旧实现 `CHackLog_InvitationPaperSellToNPC hackLog; hackLog.X()` 会多压 this
// 且引入对象槽。配套 .h：GetHackType 加 static（mangled 不变）。
// 另修：两处分派改 switch（ORIG 0x086194ea/0x08619708 线性决策树，P251/P46）；
// 删 isState5/int t 具名局部（P0 内联）；getCurCharacInvenW/R 改链式（P183）；
// cMyTrace 改全表达式临时链（P227，ORIG 实参在 ctor 前求值入 edi/esi/ebx）。
#include "Store/user_sell_item.inc"
}
// ---- repair_equip 辅助类型 ----
struct tagRepairItemInfo
{
    INVEN_TYPE m_type;       // 0=背包, 1=装备栏
    int m_slot;              // 槽位索引
    CEquipItem const* m_item;

    // ORIG C1/C2 0x0861b08e（W）：三形参顺序赋入（INVEN_TYPE, int, CEquipItem const*）。
    tagRepairItemInfo(INVEN_TYPE type, int slot, CEquipItem const* item)
        : m_type(type), m_slot(slot), m_item(item) {}
};

extern int getEquipmentRepairPrice(int baseCost, int curEndurance, int maxEndurance,
                                   int growthGrade, bool flag, float rate);

// ---------------------------------------------------------------------------
// repair_equip(CUser*, char invenType, short slot, ushort count, ushort& out) ——
// ORIG 0x08619cec（docs/class_func_reports/Store/repair_equip.md 汇编+反编译 C）。
// slot==-1：全修 —— 装备栏 3..8（invenType 1）+ 背包 10..0x15（invenType 0）扫描，
//   收集需修装备（maxEnd!=0 且 curEnd!=maxEnd，objdump +0xc00/+0xe90），累计价格后
//   use_money(totalCost, reason 1, log 1)；成功先统计(0x14)+死亡之塔(仅 state==10、
//   dungeon idx!=11000) 上报，再逐件重取槽写满耐久 update_item，最后对背包
//   10..0x15 发 SendUpdateItem(1, space 3)；失败返 10。返回 0x13=交易中。
// 单件：msg invenType 0→装备栏(type 1)/2→cargo/3→背包(type 0)，其余返 0x11；
//   空槽或非 ITEM_TYPE_EQUIP 返 0x11；*out=当前耐久；查表空/满耐久返 0x11；
//   count!=0xffff 时从 GetInvenSlot(1,count) 取工具（须 ITEM_TYPE_2 且
//   m_addInfo==0x1f77/0x201f，否则 0x15）→ use_item(count,1,0) 成功扣 1 并把
//   修理费置 0（失败 0x16）；use_money 失败返 10；耐久写满后按 msg 空间
//   update_item + SendUpdateItem(1, space 0/2/3)。
// 字段口径：判空/查表/工具 id 全程直读 +0x02 m_addInfo（item idx）；耐久为
//   +0x0b m_fieldb。m_addInfo2(+0x07, get_add_info) 是 stackable 数量域，勿混。
// ---------------------------------------------------------------------------
// [b28d-solo-Store] 判词：DIFF·N82/N85 封顶（语义完成）。
// 本轮修正（全部有 ORIG 实证）：① find_item 目标互换——第一循环（装备栏 3..8）是
// CItemList::find_item、第二循环（背包 10..0x15）是 CDataManager::find_item，旧实现写反；
// ② get_charac_level/get_endurance 逐次重读（各 +3/+4 调用点）；③ 循环内结果复用函数头
// item（4.4 对 `Inven_Item x=f()` 直用 sret 临时，N86；改赋值后逐字段拷贝形与 ORIG 一致，
// sret 临时槽 7 调用点复用同址）；④ 两处分派与 update_item 分派改 switch（ORIG 无槽
// movsbl+cmp 决策树，P251/P286）；⑤ if/else 块序交换（ORIG je 落 slot==-1 块，P59/P222）；
// ⑥ get_state/use_money/is_stackable 表达式直用（P0 无槽）；⑦ notTower 改 P158 形。
// 残余（p167 CLEAR 114=114 硬证据下）：帧 0x22c vs 0x1bc + 全帧槽号平移（N82/N95），
// rate 实参 4.1 flds/fstps 栈往返 vs 4.4 mov（N85），迭代器 operator* 返回值两段拷贝
// 与寄存器角色互换（N7/N18a）。gap = -24/1067（2.2%）。
#include "Store/repair_equip.inc"
}

// ---- force 发射 tagRepairItemInfo C1/C2（ORIG W 0x0861b08e）----
namespace {
#include "Store/forceTagRepairItemInfoEmission.inc"
}
}  // namespace
