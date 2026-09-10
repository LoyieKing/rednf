// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Redeem_Item__CRedeemItem/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Redeem_Item::CRedeemItem —— 恢复实现（独立 TU）。
// 语义依据 docs/class_func_reports/Redeem_Item__CRedeemItem/（ORIG 反汇编 + Ghidra C）。
// 仅实现 CUser::Add_RedeemInfo 调用的两个方法 + 私有辅助 _isFPItem；
// 其余 ORIG 方法（ctor/dtor/loadData/_saveData/GetItemPrice/RedeemItem 等）未建模。

#include "Redeem_Item__CRedeemItem.h"

#include "CDataManager.h"
#include "CItem.h"
#include "GameWorld.h"        // G_GameWorld / IsEnchantRevisionChannel
#include "PacketGuard.h"

namespace Redeem_Item
{

// ORIG 命名空间内 static 常量（_ZN11Redeem_ItemL...E，r 段；值实测
// 0x08cd21c8..）：MAX_COUNT=21 / THROW_FEE=1000 / BUY_FEE=1000。
static const int MAX_COUNT = 21;
static const int THROW_FEE = 1000;
static const int BUY_FEE = 1000;

// ORIG 0x085f7c04：itemId 在 (0x1963, 0x1b57] 区间 → true（"FP 物品"判定）。

#include "Redeem_Item__CRedeemItem/_isFPItem.inc"
}

// ORIG 0x085f76f0：空物品/FP 物品/固定 ID 表/物品不存在/低稀有度可升级
// 装备/禁兑换物品 → 0；否则 1。
// [b28d-46 判词·N/P168a③b] 99/101；残余唯一处：ORIG 0x085f7803
// `call IsBanRedeemItem; test %al,%al; je`（uchar 返回直判）；4.4-O0 对
// uchar callee 的条件恒插 `setne %al; test %al,%al` 归一。正解需 CItem::
// IsBanRedeemItem 返回类型 uchar→bool（mangled 不变），但定义在清单外
// CItem.cpp（R1 禁编辑）⇒ 跨版本 ABI 差封顶，语义等价。
#include "Redeem_Item__CRedeemItem/isAddableFilter.inc"
}

// ORIG 0x085f71a8：构造 Inven_Item_Expand_RedeemInfo（复制物品前 0x3c 字节 +
// 末字节 + count=param2+1000/1000），alter()，list（this+8）超 0x14 项时
// pop_front，push_back，返回 1。
#include "Redeem_Item__CRedeemItem/AddRedeemList.inc"
}

// 未达 end()），逐项整体赋值 0x44 字节到 pData + i*0x44 + 8（ORIG 为
// rep movsl 0x11，结构整体赋值即内联该复制）。
#include "Redeem_Item__CRedeemItem/getData.inc"
}


// ORIG T 0x085f73ee _ZN11Redeem_Item11CRedeemItem15Send_RedeemListEP5CUser：
// header(0,0x121)/byte(list size)/逐项（≤0x15 项）：+0x40 计数、Inven_Item
// 字段展开、附魔通道分支、增幅属性、3rd Chronicle 子包与 put_packet，最后
// finalize + Send 并返回 1。
#include "Redeem_Item__CRedeemItem/Send_RedeemList.inc"
}

}  // namespace Redeem_Item
