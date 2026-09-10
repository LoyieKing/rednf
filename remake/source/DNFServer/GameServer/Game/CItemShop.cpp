// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CItemShop/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// [b28d-38b 判词] 本 TU 9 件最终状态（check_tu_routed fresh，改前 132/262/0/10
// → 改后 132/267/0/5，净 +5 AE）：
//  checkValidShopItem 0x08512cda AE（bool 标志 + 空 then/else 守卫 + 条件处 xorb
//   （P168a④ 栈槽物化）+ 内层 while(bFlag) 单次体；与 ORIG 89/89 逐条 identical）
//  checkLimitNpcBuyItemList 0x08513580 AE（同构；逐条 identical）
//  IsOneADayShop 0x08513082 AE（ItemShopScript::m_isOneADay char→bool，
//   P292 消费端判据：ORIG 0x085130da `movzbl 0x48(%eax); test %al,%al` 无 setcc）
//  updateOneADayItemList 0x08512e98 AE（P227 全表达式临时链：ORIG 0x08512f1a 在
//   cMyTrace ctor 前把 it->second.m_shopId 载入 %ebx 跨 ctor 保活）
//  loadItemShopFiles 0x08512aa0 AE（importItemShopScript 返回类型 int→bool
//   （ORIG 0x08512b33 调用点 `xor $1`，P292）+ path 局部前置声明（ORIG 0x08512ab9））
//  setOneADayItemListNo 0x08512f9c / testupdateOneADayItemList 0x08512f88 / 
//   GetOneADayItemList 0x08513076 AE/identical
//  importItemShopScript 0x089dc5b4 DIFF（p167 CLEAR 59/59，仅 C1/C2·D1/D2 同址
//   别名 P93；残差 = P164 循环旋转 + P223 岛数（ORIG 循环头 2 个 nop 岛 / 尾部
//   5 nop+4 jmp vs 我方 4 nop+3 jmp）+ N82 槽复用（allocator -0xd vs -0x25，
//   字符串槽整体浅 4）⇒ 语义完成、byte 层 N 封顶）
//  _Z41__static_initialization_and_destruction_0ii N123 撞名豁免（体已 16/16 逐条
//   identical、尺寸 0x40 与 ORIG 全等；已去 DNFLexWrapper.h 依赖消除多余
//   _ZStL8__ioinit——其链上 DNFLex.h 含 <iostream>，改用局部权威签名声明 Scan*）
//  __uninitialized_copy ×3 N89/N130（STL 模板实例；move_iterator 版 67/54）
// df_game_r 还原 —— CItemShop（NPC 物品商店，G4 商城/交易域批次）
// 参照 docs/class_func_reports/CItemShop.md + ORIG 反汇编逐函数核对。
// 布局见 CDataManager.h；ItemShopScript 布局依据 C1/clear/import 推导。
// ============================================================================

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <ctime>
#include <memory>

#include "CDataManager.h"
#include "LogManager.h"
#include "CSystemTime.h"
#include "NpcScript.h"

// [b28d-38b] 不 include DNFLexWrapper.h：其链上 DNFLex.h 含 <iostream>，会使本 TU
// 发射 ORIG 本 TU 无有的 _ZStL8__ioinit（SIAD 0x69 vs ORIG 0x40，P338 反例）。
// 改用局部权威签名声明（mangled 与 ORIG 一致，定义在 DNFLex TU）。
#include "CItemShop/ScanType.inc"
#include "CItemShop/ScanInt.inc"
#include "CItemShop/ScanStr.inc"

namespace GlobalData
{
extern CSystemTime s_systemTime_;
}

// ---- 跨类符号（ORIG 真实符号） ----

// [L32 GI 调序] ORIG `_GLOBAL__I_g_itemShopScriptBaseDirectory`（CU=RDARScriptItemShop.cpp）
//   ⇒ 本 TU 首个强符号须为 g_itemShopScriptBaseDirectory。仅交换两块定义的先后：
//   零语义/零布局改动（g_itemShopScriptFileList 仍是本 TU 唯一动态初始化对象，
//   相对构造序不变，P327）。
// ORIG g_itemShopScriptBaseDirectory（B 0x094fe160[0x100]）：脚本目录全局
// 缓冲（initItemShopScript 0x89dc4f8 填充；本 TU 提供符号，空目录等价
// loadRDARScriptFile("", path)）。nm 权威名见 InitScriptProducers_5.cpp。
char g_itemShopScriptBaseDirectory[0x100];

STScriptFileList g_itemShopScriptFileList;  // ORIG 全局（0x94fe260），原 GameStubs.cpp 定义迁移

// [b28d-38b] 返回类型 bool：ORIG loadItemShopFiles 0x08512b33 调用点
// `xor $0x1,%eax; test %al,%al; je`（P292：无 setcc ⇒ bool；原 int 生 test+sete）。
#include "CItemShop/importItemShopScript.inc"
}

// ---- loadItemShopFiles @ 0x8512aa0 ----
#include "CItemShop/loadItemShopFiles.inc"
}

// ---- checkValidShopItem @ 0x8512cda ----
#include "CItemShop/checkValidShopItem.inc"
}

// ---- checkLimitNpcBuyItemList @ 0x8513580 ----
#include "CItemShop/checkLimitNpcBuyItemList.inc"
}

// ---- IsOneADayShop @ 0x8513082 ----
#include "CItemShop/IsOneADayShop.inc"
}

// ---- GetOneADayItemList @ 0x8513076 ----
#include "CItemShop/GetOneADayItemList.inc"
}

// ---- setOneADayItemListNo @ 0x8512f9c ----
#include "CItemShop/setOneADayItemListNo.inc"
}

// ---- updateOneADayItemList @ 0x8512e98 ----
#include "CItemShop/updateOneADayItemList.inc"
}

// ---- testupdateOneADayItemList @ 0x8512f88 ----
#include "CItemShop/testupdateOneADayItemList.inc"
}
