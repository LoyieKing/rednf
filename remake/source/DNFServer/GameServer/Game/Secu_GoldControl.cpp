// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Secu_GoldControl/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ Secu_GoldControl 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 对照 docs/class_func_reports/Secu_GoldControl.md
#include "CUser.h"
#include "GlobalData.h"
#include "SigTypes.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include <string.h>

class DB_CheckCharacName
{
public:
    static void makeRequest(int uid, unsigned int accId, const char* charName, bool flag);
};
// ORIG 金币统计区组件视图（P1：成员视图折叠 lea/add 与 ORIG 同形）
struct SecuGoldView
{
    char m_pad0[0x04];          // m_pUser @ +0x00
    user_gold_info m_total;     // +0x04
    user_gold_info m_cur;       // +0x40
    user_gold_info m_backup;    // +0x7c
};
// 协议视图（W8LT_DBTask.cpp 同型）：+0x00=accId、+0x04=char_name 串、
// +0x22=flag 字节（§4.3 具名字段直写，ORIG `mov %dl,0x22(%eax)` 直接寻址）
#pragma pack(push, 1)
struct DB_CheckCharacNameReqView
{
    unsigned int m_accId;       // +0x00
    char m_charName[0x1e];      // +0x04
    unsigned char m_flag;       // +0x22
    unsigned char m_pad23;      // +0x23
};
#pragma pack(pop)

#include "Secu_GoldControl/Init.inc"
}

#include "Secu_GoldControl/makeRequest.inc"
}
#include "Secu_GoldControl/SetInfo.inc"
}

#include "Secu_GoldControl/SetMailCharName.inc"
}

#include "Secu_GoldControl/resetRangeMoney.inc"
}
// b28d-31: N 封顶（N82附）——ORIG 0x0828757c；4.1 push %esi/%ebx 缓存
// this/&m_mailCharName vs 4.4 栈槽 -0x10/-0xc（跨 call 值缓存策略差）。
#include "Secu_GoldControl/AddGold.inc"
}
// b28d-31: N 封顶（N82附/P209）——ORIG 0x0828764a；同上 callee-saved 差，
// 且 reason 比较 ORIG `cmp $0xe,%eax`（载寄存器）vs 4.4 mem 直比。
#include "Secu_GoldControl/SubGold.inc"
}

#include "Secu_GoldControl/AddMoney.inc"
}

#include "Secu_GoldControl/SubMoney.inc"
}

#include "Secu_GoldControl/AddImportMoney.inc"
}

#include "Secu_GoldControl/SubImportMoney.inc"
}
#include "Secu_GoldControl/AddAuctionMoney.inc"
}

// ORIG 0x0828795a：value>4999 时 m_cur/m_total.m_auctionSub += value、
// m_total.m_count[1]++，再 GoldMinus(value, 2)（无 GoldTracking）。
#include "Secu_GoldControl/SubAuctionMoney.inc"
}

#include "Secu_GoldControl/MailComplete.inc"
}

#include "Secu_GoldControl/TradeComplete.inc"
}

#include "Secu_GoldControl/MailGold.inc"
}

#include "Secu_GoldControl/MailSameMid.inc"
}



#include "Secu_GoldControl/SavetoDB.inc"
}

#include "Secu_GoldControl/CheckMoneyRange.inc"
}
#include "Secu_GoldControl/GetGoldInfo.inc"
}

#include "Secu_GoldControl/CheckDate.inc"
}


#include "Secu_GoldControl/UseFatigue.inc"
}
