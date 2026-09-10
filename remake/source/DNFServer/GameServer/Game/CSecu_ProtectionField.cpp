// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CSecu_ProtectionField/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CSecu_ProtectionField 实现（独立 TU，从 GameStubs.cpp 拆出）。
#include "CInventory.h"
// [Lane37CUser 2026-09-10 解环] CInventory.h 的 `#include "CUser.h"` 已改前向声明
//   （解环）⇒ 本 TU 解引用 CUser 成员，需自行直补完整定义。
#include "CUser.h"

// ORIG T 0x08288074：枚举名 SECURITY_PROTCTION_TYPE（23 字符，c++filt 实测），
// ORIG mangled _Z23ExceptionProtectionTypei23SECURITY_PROTCTION_TYPE_S_S_S_S_S_S_S_。
enum SECURITY_PROTCTION_TYPE
{
    SECURITY_PROTCTION_TYPE_0 = 0
};

// ORIG 0x8288074：对 base 依次检查 9 个类型参数，为 0 则清对应位（bit0..bit8）。
#include "CSecu_ProtectionField/ExceptionProtectionType.inc"
}

#include "CSecu_ProtectionField/Check.inc"
}

// ORIG T 0x08288140（报告 GetOppositeErr.md）：switch 树状展开。
#include "CSecu_ProtectionField/GetOppositeErr.inc"
}


#include "CSecu_ProtectionField/GetProtectionType.inc"
}

// ORIG T 0x082881a2（报告 GetCashShopErr.md）：switch 树状展开。
#include "CSecu_ProtectionField/GetCashShopErr.inc"
}
#include "CSecu_ProtectionField/init.inc"
}

#include "CSecu_ProtectionField/CSecu_ProtectionField.inc"
}

// ORIG 0x0828873a（_ZN21CSecu_ProtectionField9CheckItemEP5CUser18SECURITY_PROTCTIONm）：
// 物品 index 命中受保护集合（0x1f70/0x1f71/0x989a13/0x292090e1）时委托 Check(user,type)，
// 否则直接 0。
#include "CSecu_ProtectionField/CheckItem.inc"
}

// ORIG 0x08288786（_ZN21CSecu_ProtectionField9CheckItemEP5CUser18SECURITY_PROTECTIONPK5CItem）：
// item/user 任一为空则 1，否则取 item->get_index() 走上面的重载。
#include "CSecu_ProtectionField/CheckItem_1.inc"
}

// ORIG T 0x08288bf2（报告 CheckRestoreType.md）：恢复惩罚位检查。
#include "CSecu_ProtectionField/CheckRestoreType.inc"
}

// ORIG T 0x08288c46（报告 CheckTradingType.md）：交易惩罚位逐位检查。
#include "CSecu_ProtectionField/CheckTradingType.inc"
}

// ORIG T 0x08288dbc（报告 CheckEtcType.md）：type∈{8,9} 且安全服务未启用 → 0xe9。
#include "CSecu_ProtectionField/CheckEtcType.inc"
}

// ===========================================================================
// LinkMisc5 收口（ORIG 反汇编/报告逐条转写）
// ===========================================================================

// ORIG T 0x082886ae _ZN21CSecu_ProtectionField15GetInvenItemPtrEP5CUsercs：
// space==2 → 仓库槽引用；space∈{0,3,7} → 背包槽引用；其余返回空。
#include "CSecu_ProtectionField/GetInvenItemPtr.inc"
}
// ORIG T 0x082884d6 _ZN21CSecu_ProtectionField10GetItemPtrEP5CUsercs：
// 取槽位物品副本后按 m_addInfo 查模板对象。
#include "CSecu_ProtectionField/GetItemPtr.inc"
}

// ORIG T 0x08288ac8 _ZN21CSecu_ProtectionField16CheckAccountTypeEP5CUserm：
// 密保卡/密保 Pad 认证门 + 交易惩罚位检查。
#include "CSecu_ProtectionField/CheckAccountType.inc"
}

// ORIG T 0x08288994（报告 CheckOpposite.md）：先做自身 Check，再核对交易对手惩罚态。
#include "CSecu_ProtectionField/CheckOpposite.inc"
}

// ORIG T 0x082887d0 _ZN21CSecu_ProtectionField15CheckItemRarityEP5CUser18SECURITY_PROTCTIONPK5CItemPK10Inven_Item
#include "CSecu_ProtectionField/CheckItemRarity.inc"
}
