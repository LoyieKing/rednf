// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 GmCmds_C/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// GmCmds_C.cpp — game_master 命令体系批次 C（df_game_r ORIG T 符号族还原）。
// 负责类（10）：
//   CCraneNeedMaterial        ctor 0x082a86aa / execute 0x084b0626
//   CCreateDisjointStoreCmd   ctor 0x082a8108 / execute 0x084a9720
//   CDeleteAccountCargo       ctor 0x082a84ce / execute 0x084abdb4
//   CDeleteGuild              ctor 0x082a83e0 / execute 0x084ab29c
//   DungeonDifficulty         ctor 0x082a8892 / parse 0x084b2110 / execute 0x084b2152
//   CEndEvent                 ctor 0x082a845e / parse 0x084ab914 / execute 0x084ab94c
//   CEnduranceCmd             ctor 0x082a83fc / parse 0x084aa73a / execute 0x084aa858
//   CGrowhWeaponEvent         ctor 0x082a88e6 / parse 0x084b28d6 / execute 0x084b293c
//   CGuildDebug               ctor 0x082a8592 / parse 0x084ac720 / execute 0x084ac76c
//   CGuildExpBookDelete       ctor 0x082a8506 / execute 0x084abf2c
// 对照 docs/class_func_reports/game_master__<Class>/{parse,execute}.md。
//
// 头文件占位字段失配说明：GameMasterCmd.h 对本批各类统一给出 m_str0(string@+0x08)
// 等占位名，但 ORIG 真实布局按各类不同：
//   CEndEvent          +0x08 int（事件代码）
//   CEnduranceCmd      +0x08 int（背包槽位）+0x0c unsigned short（耐久值）
//   CGrowhWeaponEvent  +0x08 byte（"gw" 重置标记）+0x0c int（时计数值）
//   DungeonDifficulty  +0x08 byte（难度 1..3）
//   CGuildDebug        +0x08 int +0x0c int +0x10 unsigned int（m_f1 占位即此字段）
//   （gcc4.4 32 位 std::string 仅 4 字节，故占位 string 与上述标量布局同尺寸。）
// 本 TU 一律按偏移访问真实标量字段，不向占位 std::string 写入。
#include "GameMasterCmd.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "GameWorld.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CInventory.h"
#include "CServerProxyMgr.h"
#include "Inven_Item.h"
#include "CDungeon.h"
#include "CDungeonClear.h"
#include "CDataManager.h"
#include "CExpertJob.h"
#include "CGuildServerProxy.h"
#include "CMonitorServerProxy.h"
#include "STItemScript.h"
#include "Packet_Monitor_Notice_Guild_Dismiss.h"
#include "Packet_Monitor_Event_End.h"
#include "Packet_Guild_Debug.h"
#include "CEnvironment.h"
#include "DB_EventGrowthWeaponReset.h"            // makeRequest（桥清零）
#include "DB_EventUpdateGrowthWeaponTimepiece.h"  // makeRequest（桥清零）

#include <stdlib.h>

namespace game_master
{

// ========================================================================
// CCraneNeedMaterial —— sizeof=0x8，无数据字段。
// ========================================================================

// ORIG ctor 0x082a86aa：基类 vptr → 自身 vptr。

// ORIG execute 0x084b0626：G_GameWorld()->UpdateMiniCraneSeed()。
#include "GmCmds_C/execute.inc"
}

// ========================================================================
// CCreateDisjointStoreCmd —— sizeof=0x8，无数据字段。
// ========================================================================

// ORIG ctor 0x082a8108。

// ORIG execute 0x084a9720：取分解师职业槽挂到 GM 角色，随后在其脚下
// 以固定名 "test"/容量 500 创建分解商店（坐标取 GM 角色当前位置低 16 位）。
// 【b28 判词 N-P75】job 槽(-0xc)值供 SetCurCharacExpertJob 实参：ORIG 4.1 在
// GetUser call 前物化入 %ebx（`mov -0xc(%ebp),%ebx` 前置），4.4 恒 call 后
// 重读槽入 %edx（参数装载时刻跨版本调度，P75；P226 语句表达式形在纯槽装载
// 场景不升格，探针 /tmp/bm3/p13 fSE），余 25 条逐条相等。
#include "GmCmds_C/execute_1.inc"
}

// ========================================================================
// CDeleteAccountCargo —— sizeof=0x8，无数据字段。
// ========================================================================

// ORIG ctor 0x082a84ce。

// ORIG execute 0x084abdb4：CAccountCargo::DeleteAccountCargo(GetUser())。
// ORIG 该函数为静态语义直调（cdecl，无 this 使用）；此处经 CUser 内嵌实例
// （布局注记 CUser.h：+0x6effc CAccountCargo）走同一实现路径。
#include "GmCmds_C/execute_2.inc"
}

// ========================================================================
// CDeleteGuild —— sizeof=0x8，无数据字段。
// ========================================================================

// ORIG ctor 0x082a83e0。

// ORIG execute 0x084ab29c：GM 校验通过后向 guild 服务器发解散请求（0x1df 流）、
// 清本地公会键并回发 0x3d 包，同时向 monitor 广播 Packet_Monitor_Notice_Guild_Dismiss。
#include "GmCmds_C/execute_3.inc"
}

// ========================================================================
// DungeonDifficulty —— sizeof=0xc；+0x08 byte 难度（占位 m_str0 失配，偏移访问）。
// ========================================================================

// ORIG ctor 0x082a8892。

// ORIG parse 0x084b2110：难度字节清零后 atoi(args) 写入 +0x08。
#include "GmCmds_C/parse.inc"
}

// ORIG execute 0x084b2152：遍历全地下城索引表，为非特殊地下城写入通关难度记录。
#include "GmCmds_C/execute_4.inc"
}

// ========================================================================
// CEndEvent —— sizeof=0xc；+0x08 int 事件代码（占位 m_str0 失配，偏移访问）。
// ========================================================================

// ORIG ctor 0x082a845e。

// ORIG parse 0x084ab914：atoi(args) → +0x08。
#include "GmCmds_C/parse_1.inc"
}

// ORIG execute 0x084ab94c：GM 校验后向 monitor 发 Packet_Monitor_Event_End。
#include "GmCmds_C/execute_5.inc"
}

// ========================================================================
// CEnduranceCmd —— sizeof=0x10；+0x08 int 槽位、+0x0c unsigned short 耐久
// （占位 m_str0 失配，偏移访问）。
// ========================================================================

// ORIG ctor 0x082a83fc。

// ORIG parse 0x084aa73a：trim 后以首个空格切分 args；
// 前段 atoi → 槽位(+0x08)，后段 atoi → 耐久(+0x0c)。ORIG 未校验 find 失配。
#include "GmCmds_C/parse_2.inc"
}

// ORIG execute 0x084aa858：GM 且槽位 ∈ (9,0x15]（cmp $0x15;jg 允许 ==0x15）时改写背包物品耐久并回发更新包。
#include "GmCmds_C/execute_6.inc"
}

// ========================================================================
// CGrowhWeaponEvent —— sizeof=0x10；+0x08 byte "gw" 重置标记、+0x0c int 时计值
// （占位 m_str0 失配，偏移访问）。
// ========================================================================

// ORIG ctor 0x082a88e6。

// ORIG parse 0x084b28d6：args=="gw" 时置重置标记，否则标记清零并 atoi 存时计值。
#include "GmCmds_C/parse_3.inc"
}

// ORIG execute 0x084b293c：改写成长武器事件数据并落库——
//   标记置位：整体清零事件数据 + DB_EventGrowthWeaponReset(accId)；
//   否则：写时计值/清计数 + DB_EventUpdateGrowthWeaponTimepiece(accId, value, 0)。
#include "GmCmds_C/execute_7.inc"
}

// ========================================================================
// CGuildDebug —— sizeof=0x14；+0x08/+0x0c int 参数、+0x10 unsigned int（m_f1 占位）。
// ========================================================================

// ORIG ctor 0x082a8592。

// ORIG parse 0x084ac720：atoi(args) → +0x08；+0x0c、+0x10 显式清零。
#include "GmCmds_C/parse_4.inc"
}

// ORIG execute 0x084ac76c：组 Packet_Guild_Debug 经 guild 代理 SendTcpPacket
//（ORIG 无 isGMUser 校验）。size 取包内 0x1e。
#include "GmCmds_C/execute_8.inc"
}

// ========================================================================
// CGuildExpBookDelete —— sizeof=0x8，无数据字段。
// ========================================================================

// ORIG ctor 0x082a8506。

// ORIG execute 0x084abf2c：GetUser()->deleteGuildExpBook()。
#include "GmCmds_C/execute_9.inc"
}

} // namespace game_master