// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CUserStatC/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CUserStatC.cpp —— CUser 物品域三符号（Prod_CUserC 批次）
//
//   CUser::delete_item                 ORIG T 0x0866285c _ZN5CUser11delete_itemEcimi14eItemDelReason
//   CUser::exchange_interspace         ORIG T 0x0865f7b0 _ZN5CUser19exchange_interspaceEiimiiimPi
//   CUser::exchange_interspace_expand  ORIG T 0x0865f614 _ZN5CUser26exchange_interspace_expandEiimiiimPicc
//
// 依据 docs/class_func_reports/CUser/{delete_item,exchange_interspace,
// exchange_interspace_expand}.md 的 ## 汇编 + ## 反编译 C 全文逐分支转写；
// 每个被调地址经 df_game_r nm 解析为真实符号后映射到权威头声明：
//   CheckInTrade/getCurCharacInvenR/W/getCurCharacCargoR/W/get_acc_id
//   GetInvenTypeFromItemSpace/Inven_Item::{C2,set_add_info,get_add_info}
//   IsExistAccountCargo/GetAccountCargo/CTradeSpace::{is_empty,IsLocked}
//   getCurChannelEquipslotSwitch/getCurExpandEquipslotSwitch/G_GameWorld
//   GameWorld::IsCheckUnusableEquipmentChannel/CAvatarItemMgr::IsTempKey(int)
//   change_equip/change_equip_expand/_checkItemMovable(声明先行，定义后续批次)
//   cUserHistoryLog::{ItemAddCargo,AccountCargoAddItem,AccountCargoDelItem}
//   online_preliminary::COnlinePreliminary::CheckUnusableEquipment(T 0x08560920)
// 虚调用 vtable+0x40 经 ORIG vtable(CStackableItem@0x08c96ea0) 槽位转储核实
//   = CItem::IsImpossibleCharacCargo() const。
// ============================================================================

#include "CUser.h"
#include "CDataManager.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "CAvatarItemMgr.h"
#include "CTradeSpace.h"
#include "CGameManager.h"
#include "GameWorld.h"
#include "CParty.h"
#include "ServiceRestrictManager.h"
#include "GlobalData.h"
#include "RDARScriptStringManager.h"
#include "PacketGuard.h"

// ============================================================================
// CUser::delete_item(char space, int position, unsigned long itemIdx,
//                    int count, eItemDelReason reason)
// ORIG 0x0866285c sz 0x25d。返回：0 成功 / 0x11 校验或删除失败 / 0x13 交易中。
// [b28d-solo-CUserStatC 判词·AE] P167 CLEAR（17/17），orig 162 = our 162 逐条对齐
//   （仅跳表地址/jump-table 重定位属 AE 口径）。本件修法：
//   ① 0x086628c1 比较：ORIG 为 4.1 bool 寄存器物化 `setne %al; test %al,%al; je`，
//      if 形 4.4 恒折 `je` 直跳 ⇒ 改 P225 形 `while (cond) { return 0x11; }`
//      （探针 /tmp/b28d_cstatc_probe2.cpp fJ 逐条命中）。
//   ② switch 叶块物理序 = case 书写序（P46）：ORIG 序 case0,case2,case3,case1,case7
//      ⇒ 源码 case 1 移至 case 3 之后、case 7 之前（原序 case0,1,2,3,7 致块错位）。
// ============================================================================
#include "CUserStatC/delete_item.inc"
}

// ============================================================================
// CUser::exchange_interspace(...)
// [b28d-solo-CUserStatC 判词·DIFF→N 封顶] §6.2 三件套：①P167 CLEAR 265/265
//   [0 缺 0 多]；②gap 2637−2619 = 18（our−18）；③帧/槽：ORIG `sub $0x45c` +
//   push edi/esi/ebx vs 我们 `sub $0x490` + push ebx/esi（N82 槽复用/帧分区族）。
//   本波次修正（语义级，均已 fresh 复编核销）：
//   ① 真语义 bug：case 3→7 的 GetInvenRef 实参原写 INVEN_TYPE_INVEN(0)；
//      ORIG 0x08660ec4 为 INVEN_TYPE_CREATURE(3)（目标为宠物槽）——已修。
//   ② 5 处 `check_trade_possibility() == 1` 改反极性 `if (!f())`（P272/P292）：
//      ORIG 为 `call; xor $0x1; test %al,%al; je <then>`，`==1` 形 4.4 恒产 cmp $1。
//   ③ 13 处 bool callee 的 `!= 1`/`== 1` 改 `!f()`/真值（P292 档①）：
//      CCargo::check_slot_empty、CAccountCargo::{CheckInsertCondition,CheckSlotEmpty,
//      CheckStackLimit} 声明+定义 int→bool（已 R2 广播；CUser.h/CAccountCargo.cpp
//      /CCargo.cpp 定义端由 Main 代落同步）。
//   ④ case 1 源两组 if-else 链、case 7 源 if-else 链改 switch（P286/P208 推广）：
//      ORIG 为 `mov 0x1c(%ebp),%eax; cmp $K,%eax; je` 载入寄存器比较链，4.4 对
//      if 链恒折内存直比；switch 按 case 书写序复现（case1: 3,4,1；case7: 4,7,3）。
//   ⑤ case 3→1 的 `ret < 1` 改正守卫早返（ORIG 0x0866168a 为 `cmpl $0; jle <then>`，
//      `ret<1`/`ret<=0` 均使 4.4 反产 `jg`）。
//   ⑥ 5 处 move_item 返回检查改 P225 形 `while (ret != 0) { return ret; }`：
//      ORIG 为 `cmpl $0x0,槽; setne %al; test %al,%al; je`（4.1 bool 物化），
//      if 形 4.4 恒折 `cmpl; jne/je`。
//   ⑦ 2 处 `if (!delOk)` 反写为 `if (delOk) {成功} else {trace}`（N122：4.4 对 bool
//      局部取反恒加 `movzbl; xor $1`；ORIG 为 `mov %al,槽; cmpb $0x0,槽; je <失败>`）。
//   ⑧ CCargo::move_item / CAccountCargo::DeleteItem int→bool（P292 档①；定义端
//      CCargo.cpp:376 / CAccountCargo.cpp:247 由 Main 同步）。
//   [b28d-BigCUserStat 2026-09-09] 旧判词「4.1 shr $0x1f 符号位 lowering 六形全败」
//   作废：4.4.7 探针 /tmp/sign_probe2.cpp f13 实证 `(unsigned char)(((unsigned int)x) >> 31)`
//   精确复现 ORIG `mov; shr $0x1f,%eax; test %al,%al; je`（语义与 `x < 0` 等价，
//   P310 扩展）——10 处 toSlot<0 判定全部改写 ⇒ ours 2618→**2638**，gap +20→**0**；
//   struct 452→412、ndiff 1698→1658；TU 四数 0/3/0/2/1 不变。
//   残余：exist 两处 `if (exist >= 0)` 仍 js/jns（ORIG 0x0865fb10/0x0866228a 为 shr 形，
//   但按 shr 形改写后 then/else 布局多 1 jmp/处，实测 2640 ⇒ 指令数优先保留 2638）+
//   N82 帧/槽（0x45c vs 0x490）+ 跳表数据地址（AE 伪影）。
// ORIG 0x0865f7b0 sz 0x2d47。参数（报告语义）：fromSpace/fromSlot/fromItemIdx/
//   addInfo(数量或强化附加)/toSpace/toSlot/toItemIdx/outSlot。
// 返回：0 成功 / 1 参数域不支持 / 4 目标槽不可用 / 0x11 通用失败 /
//       0x13 交易中 / 0x15 movable 校验失败或交易表空 / 0x17 物品限制 /
//       0x5e regist_item==-4 / 0xd1 服务限制。
// ============================================================================
#include "CUserStatC/exchange_interspace.inc"
}

// ============================================================================
// CUser::exchange_interspace_expand(...)
// ORIG 0x0865f614 sz 0x19b。扩容槽位切换包装：ChannelEquipslotSwitch 开启时
// 目的域 3↔0x12 互换校验；双开关状态与请求一致才允许 change_equip_expand。
// 参数序按 ORIG mangled EiimiiimPicc：i,i,m,i,i,i,m,Pi,c,c。
// ============================================================================
#include "CUserStatC/exchange_interspace_expand.inc"
}
