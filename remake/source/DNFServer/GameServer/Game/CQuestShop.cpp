// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CQuestShop/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// CQuestShop ORIG-only 符号恢复（Wave8 W8_questshop）。
// 反汇编依据 df_game_r objdump（0x85eed2a..0x85efd73）；ctor/dtor 见
// docs/class_func_reports/CQuestShop/ 三份报告。
// df_game_r CQuestShop::clearQP 恢复。
// 依据 docs/class_func_reports/CQuestShop.md 的反汇编与反编译报告。
#include "CUser.h"
#include "CQuestShop.h"
#include "PacketGuard.h"
#include "GlobalData.h"

// CQuestShop 消费侧 statusCounts 视图（CHARAC_QUEST_SHOP_DATA 0xaa 的
// 计数视角：+0x0c..+0xa5 为 0x4d 个 ushort 状态已购计数；DB 侧 _LoadQuestShop
// 同一区按属性具名填充，两视角布局一致）。
struct QuestShopStatusView
{
    unsigned int m_pad0;              // +0x00
    unsigned int m_qp;                // +0x04
    unsigned int m_initCount;         // +0x08
    unsigned short m_statusCounts[0x4d];  // +0x0c..+0xa5
    unsigned int m_questPiece;        // +0xa6
};

#include "CInventory.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
// 顶层 stStatusLevelInfo / stQuestShopStatus 类型定义已上移至 CDataManager.h
// （全局作用域，ORIG mangled 无 stQuestShop 前缀）；本 TU 仅保留方法定义
// （C1 0x891b948 / clear 0x891b8cc / D1 0x83cd826 / C1ERKS_ 0x85f07fe /
// getMaxCount / getNextLevel，见文件后部）。
// ORIG C1 0x85eebc8 size 0x97：CData 基类（vptr）、map@+0x08、map@+0x20 构造，
// _reset() 收尾；异常清理路径按 ORIG try/leave 版式由编译器生成。
#include "CQuestShop/CQuestShop.inc"
}

// ORIG D1 0x85eec60 size 0x89：vptr 重置 → ~map(+0x20) → ~map(+0x08) → ~CData；
// D0（deleting）由编译器按虚析构规则产出（+operator delete，D1 后 delete this）。
#include "CQuestShop/dtor_CQuestShop.inc"
}

#include "CQuestShop/getQp.inc"
#include "CQuestShop/getInitQp.inc"
#include "CQuestShop/getInitCount.inc"
#include "CQuestShop/getPiece.inc"

#include "CQuestShop/setQp.inc"
#include "CQuestShop/setInitQp.inc"
#include "CQuestShop/setInitCount.inc"
#include "CQuestShop/setPiece.inc"
#include "CQuestShop/addQp.inc"
#include "CQuestShop/subQp.inc"
#include "CQuestShop/incInitQp.inc"
#include "CQuestShop/incInitCount.inc"
#include "CQuestShop/addPiece.inc"

#include "CQuestShop/clearBuyingStatuslist.inc"
}

#include "CQuestShop/sendCharacQp.inc"
}

#include "CQuestShop/sendCharacQuestPiece.inc"
}

#include "CQuestShop/clearQP.inc"
}

#include "CQuestShop/makePacket_BuyingStatusList.inc"
}

// ============================================================================
// ---- CQuestShop::subQp(unsigned char, unsigned char)  ORIG T 0x85eed2a
#include "CQuestShop/subQp_1.inc"
}

// ---- ORIG T 0x85eeda8 size 0xc2：查 [quest status] 表中该 status 的 QP ---
#include "CQuestShop/findStatusQp.inc"
}

// ---- ORIG T 0x85eee6a size 0xbe：查该 status 的槽位类型 -------------------
// （stQuestShopStatus +0x1c dword；表外返回哨兵 0x4d）
#include "CQuestShop/findStatusType.inc"
}

// ---- ORIG T 0x85ef02e size 0xa0：读 m_buyingStatus[type] -----------------
#include "CQuestShop/getStatusBuyingCount.inc"
}

// ---- ORIG T 0x85eef28 size 0x105：alter() 后累加/新建购买数 --------------
#include "CQuestShop/incStatusBuyingCount.inc"
}

#include "CQuestShop/getMaxCount.inc"
}

#include "CQuestShop/getNextLevel.inc"
}

// ORIG clear 0x891b8cc（objdump 直转）：4×string clear + +0x10=-1、+0x14=0、
// +0x18=0x4d、+0x1c=2、+0x20=0、vector clear。
#include "CQuestShop/clear.inc"
}

// ORIG C1 0x891b948（objdump 直转）：4×string + vector 构造后 clear()。
#include "CQuestShop/stQuestShopStatus.inc"
}

// ORIG C1ERKS_ 0x85f07fe（objdump 直转）：4×string + 标量 + vector 复制。
#include "CQuestShop/stQuestShopStatus_1.inc"
}

// ORIG D1 0x83cd826（objdump 直转）：vector + 4×string 逆序析构。
#include "CQuestShop/dtor_stQuestShopStatus.inc"
}

// ---- ORIG T 0x85ef0ce size 0xec：限购等级校验 ----------------------------
#include "CQuestShop/checkBuyingStatus_LimitLevel.inc"
}

// ---- ORIG T 0x85ef1ba size 0x5a：initCost 基础公式 ------------------------
#include "CQuestShop/getInitCost.inc"
}

// ---- ORIG T 0x85ef214 size 0xf1：已用 QP 汇总 -----------------------------
#include "CQuestShop/calUsedQp.inc"
}

// ---- ORIG T 0x85ef306 size 0x11f：购买状态主流程 --------------------------
#include "CQuestShop/buyStatus.inc"
}

// ---- ORIG T 0x85ef426 size 0x125：花金币重置 QP ---------------------------
#include "CQuestShop/clearUsedQp.inc"
}

// ---- ORIG T 0x85efd74 size 0x4c：_reset 虚槽 7 -----------------------------
#include "CQuestShop/_reset.inc"
}

// ---- ORIG T 0x85efdc0 size 0x94：装入 tag 状态计数 ------------------------
#include "CQuestShop/SetTagCharacQuestShopData.inc"
}

// ---- ORIG T 0x85efa2c size 0x13a：loadData 虚槽 1 --------------------------
#include "CQuestShop/loadData.inc"
}

// ---- ORIG T 0x85efb66 size 0x20e：_saveData 虚槽 6（入 DB_Q）--------------
#include "CQuestShop/_saveData.inc"
}

// ---- ORIG T 0x85ef868 size 0x1c4：getData 虚槽 2（双实例导出）-------------
#include "CQuestShop/getData.inc"
}

// ---- ORIG W 0x0891b8b4：顶层 stStatusLevelInfo C1（+0x01 清零后拷至 +0x00）
// 合法 force-use 发射（vector<stStatusLevelInfo> 元素构造点）。 ----
namespace {
#include "CQuestShop/emitStStatusLevelInfoWeakSymbols.inc"
}
}  // namespace
