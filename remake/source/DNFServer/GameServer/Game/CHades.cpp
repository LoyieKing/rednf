// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CHades/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— XNuclear::CHades（Hades 反外挂客户端成员类，内嵌于 CUser +0x8e954）。
// 参照 docs/class_func_reports/XNuclear__CHades/：
//   Send_Login           @ ORIG 0x084b9200
//   needLogin            @ ORIG 0x084b9718
//   GetIP                @ ORIG 0x084b9774
//   Send_UserStatus      @ ORIG 0x084b97fe
//   Send_ReturnToVillage @ ORIG 0x084b9386（纯空函数）
#include "CUser.h"
#include "CMCAPManager.h"

#include <string.h>

#include "CEnvironment.h"
#include "CHadesServerProxy.h"
#include "DB_InterDeps.h"   // DB_HadesPunish::makeRequest（ORIG T 0x08445dd4）
#include "GlobalData.h"
#include "Packet_Hades_Login.h"
#include "Packet_Hades_StdUpdate.h"
#include "Packet_Hades_GoldTracking.h"
#include "Packet_Hades_UserStatus.h"
#include "Packet_Hades_Logout.h"
#include "Packet_Hades_AddHackType.h"
#include "Packet_Hades_Gold_Plus.h"
#include "Packet_Hades_Gold_Minus.h"

namespace XNuclear
{

// ORIG 0x084b91e8（Init.md）：挂接 user 指针，清零疲劳计数。
#include "CHades/Init.inc"
}

// ORIG 0x084b92ea（Send_Logout.md）。Packet_Hades_Logout（ORIG C1 0x84b996a W，
// 0xfd3/0xe）：+0x0a acc_id；经 proxy SendHadesTcpPacket 发送。
#include "CHades/Send_Logout.inc"
}

// ORIG 0x084b9322（Send_HackType.md）：纯空函数（5 指令）。
#include "CHades/Send_HackType.inc"
}

// ORIG 0x084b9328（AddHackTypeCnt.md）。Packet_Hades_AddHackType（ORIG C1
// 0x84b998e W，0xfd7/0x1a）：+0x0a acc_id、+0x0e (u16)hackType、+0x10 (u16)count、
// +0x12 param3、+0x16 param4；经 proxy 发送。
#include "CHades/AddHackTypeCnt.inc"
}

// ORIG 0x084b95a8（ChangeSecurity.md）：仅转调 needLogin()，参数未参与逻辑。
#include "CHades/ChangeSecurity.inc"
}

// ORIG 0x084b93f4（Send_ChangeSecurity.md）：纯空函数（5 指令）。
#include "CHades/Send_ChangeSecurity.inc"
}

// ORIG 0x084b93fa（GoldPlus.md）。Packet_Hades_Gold_Plus（ORIG C1 0x84b99b2 W，
// 0xfda/0x14）：+0x0a acc_id、+0x0e gold、+0x12 (u16)reason；经 proxy 发送。
#include "CHades/GoldPlus.inc"
}

// ORIG 0x084b944a（GoldMinus.md）。Packet_Hades_Gold_Minus（ORIG C1 0x84b99d6 W，
// 0xfdb/0x14）：+0x0a acc_id、+0x0e gold、+0x12 (u16)reason；经 proxy 发送。
#include "CHades/GoldMinus.inc"
}

#include "CHades/Send_ReturnToVillage.inc"
}

// ORIG 0x084b953e（ExpUp.md）：仅转调 needLogin()，参数未参与逻辑。
#include "CHades/ExpUp.inc"
}

// ORIG 0x084b9718：缓存(m_connSec) 与 proxy 当前连接秒数不一致则重登并同步 cate=3。
#include "CHades/needLogin.inc"
}

// ORIG 0x084b938c。Packet_Hades_StdUpdate（ORIG C1 0x84b9a42 W，0xfd9/0x12）：
//   +0x0a acc_id、+0x0e cate、+0x10 type；经 proxy SendHadesTcpPacket 发送。
#include "CHades/StdCateUpdate.inc"
}

// ORIG 0x084b9200（Send_Login.md）：刷新连接秒缓存，构造 Packet_Hades_Login
// （ORIG C1 0x84b9946 W，0xfd2/0x64）：+0x0a acc_id、+0x5f channel_no(字节截断)、
// +0x0e ip[16]（先零化局部缓冲 → GetIP → strcpy）、+0x1e mac strncpy 0x41、
// +0x60 uid；发送后转调 Send_UserStatus()。
#include "CHades/Send_Login.inc"
}

// ORIG 0x084b9774（GetIP.md）：peer IP 抓取失败（空串）且 CMCAPManager 内
// PC房 IP(+0x28) 非空时，清零输出缓冲后拷贝该 IP。
#include "CHades/GetIP.inc"
}

// ORIG 0x084b97fe（Send_UserStatus.md）。Packet_Hades_UserStatus（ORIG C1
// 0x84b9a1e W，0xfdd/0x12）：+0x0a acc_id、+0x0e 状态位域——getSecuType 各位
// 逐次读取：&(1|2|0x20) 任一命中→|1；&8→|2；&0x10→|4。
#include "CHades/Send_UserStatus.inc"
}

// ORIG 0x084b9552（UseFatigue.md）：needLogin 后计数 +1；累计 >1 时上报 cate=1 并清零。
#include "CHades/UseFatigue.inc"
}

// ORIG 0x084b949a（GoldTracking.md）：needLogin 后构造 Packet_Hades_GoldTracking
// （ORIG C1 0x84b99fa W，0xfdc/0x29）：+0x0a acc_id、+0x0e money、+0x12 ip[16]
// （显式 memset 0x10，NULL 跳过 strncpy）、+0x22 param4、+0x26 (u16)fatigue、
// +0x28 type；经 proxy 发送。
#include "CHades/GoldTracking.inc"
}

// ORIG 0x084b93ee（Send_ChangeGold.md）：纯空函数（push ebp/mov/pop/ret，5 指令）。
#include "CHades/Send_ChangeGold.inc"
}

// ORIG 0x084b95bc（RunHadesAction.md）：Hades 行动分发。
//  a1==0x40 跳过历史日志；a1&2 → 标记 hack + 交易惩罚 + DB 上报(2)；
//  a1&1 且非 hack 计时中 → 标记 hack + 开计时 + DB 上报(1)；
//  a1&4 → DisConnSig(0x39, true, 0)。
#include "CHades/RunHadesAction.inc"
}

// ---- CHades_GameSvr（ORIG 空标记类型）----
// ORIG C1/C2 @ 0x082a5a68、D1/D2 @ 0x082a5a6e（各 5 字节 = push ebp/mov esp/pop
// ebp/ret 空体）。GameSvr 侧配置占位；无成员、无基类、无虚函数。
// 全局对象 s_hades_（_ZN10GlobalData8s_hades_E @ 0941f728）ctor 由 ORIG
// __static_initialization_and_destruction_0（0x082a3a5a）调用并注册 atexit。
class CHades_GameSvr
{
public:
    CHades_GameSvr();
    ~CHades_GameSvr();
};

#include "CHades/CHades_GameSvr.inc"
}

#include "CHades/dtor_CHades_GameSvr.inc"
}

}  // namespace XNuclear
// GlobalData 静态成员（namespace 形态，见 GlobalData.h 注释）；完整类型见上。
XNuclear::CHades_GameSvr GlobalData::s_hades_;
