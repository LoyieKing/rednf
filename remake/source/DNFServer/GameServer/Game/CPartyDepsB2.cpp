// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CPartyDepsB2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// CPartyDepsB2.cpp —— CParty 第二组（独占 TU，2026-08-24）。
// 逐函数依据 docs/class_func_reports/CParty/ 完整报告（## 汇编 + ## 反编译 C）：
//   _Send_TOD_APCInfo.md            ORIG T 0x085bac26
//   _onGetItem.md                   ORIG T 0x085b949c
//   checkDungeonDifficulty.md       ORIG T 0x085be932
//   checkInoutConditionDungeon.md   ORIG T 0x085abc80（三参重载；无参 0x85ac59c 在 CParty.cpp）
//   choose_random_buff.md           ORIG T 0x0859b814
//   die_user.md                     ORIG T 0x085a7828
//   finish_loading.md               ORIG T 0x085b15e0
//   getMemberMaxLevelBeforeDungeon.md ORIG T 0x085b613e
//   get_party_seatno.md             ORIG T 0x085a277a
//   giveup_game.md                  ORIG T 0x085b2baa
// 字段全部使用 CParty.h 具名成员；控制流按 ORIG 汇编形态逐段转写：
// 顺序守卫代替 && 链、单出口析构点、比较树/短路次序一一对应。
// 依赖的跨域叶子实现随本批次落地：
//   QuickParty::CQuickPartyRewardManager::get_random_buff_prob (0x0826d860)
//   EventClassify::CEventScriptMng::process_add_item_count_db  (0x0810bea8)
//   CSpecialItemRoutingManager::ProcessGiveUp                  (0x0860b2c2)
//   TOD_Layer::TOD_Layer / TowerOfDespairMgr::SendAPCInfo      (TowerOfDespairMgr.cpp)

#include <string.h>

#include <algorithm>

#include "GameRand.h"                   // get_rand_int
#include "QuickParty.h"                 // 需先于 CParty.h（守卫次序，同 CParty.cpp）
#include "CParty.h"
#include "CDungeon.h"
#include "BattleData.h"   // finish_loading 装载标志 m_arr7e4（CParty +0xb10）
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"
#include "LogManager.h"                 // logFormat
#include "HistoryLog.h"                 // WriteDie / WriteGiveUp
#include "CSystemTime.h"                // GlobalData::s_systemTime_ / getCurSec
#include "Secu_HackLogCheckByParty.h"   // dungeonFail
#include "CDataManager.h"               // G_CDataManager / find_item / get_event_script_mng
#include "CGameManager.h"               // G_CGameManager / GetQuickPartyRewardManager /
                                        // GetSpecialItemRoutingManager
#include "CBattle_Field.h"              // GetDungeon / IsFirstEnterDungeon / check_start_point /
                                        // get_dungeon_diff / m_villageMonsterId / m_battleField.m_dungeonClearTracer
#include "GameWorld.h"                  // G_GameWorld / out_from_dungeon / send_all
#include "CUser.h"                      // Send/make_basic_info/getHackAnalyzer/getMoveSpace/...
#include "CUserCharacInfo.h"            // getCurCharacStamina/getCurCharacNo/... （经 CUser 继承）
#include "CDungeonEntranceLog.h"        // GetInstanceDungeonEntranceLog
#include "GlobalData.h"                 // s_pTowerOfDespairMgr / s_systemTime_
#include "CEventScriptMng.h"            // EventClassify::CEventScriptMng
#include "Statistics.h"                 // Statistics::DungeonFail（static ABI）
#include "CSpecialItemRoutingManager.h" // ProcessGiveUp
#include "TowerOfDespairMgr.h"          // TOD_Layer / TowerOfDespairMgr

// ============================================================================
// CParty::get_party_seatno —— ORIG 0x085a277a
// ============================================================================
#include "CPartyDepsB2/get_party_seatno.inc"
}

// ============================================================================
// CParty::getMemberMaxLevelBeforeDungeon —— ORIG 0x085b613e
// ============================================================================
#include "CPartyDepsB2/getMemberMaxLevelBeforeDungeon.inc"
}

// ============================================================================
// CParty::die_user —— ORIG 0x085a7828
// [N96 清绑 2026-09-09] 原 `register unsigned char live asm("eax")` 全域禁用
// （P0②/N96/P248 尾条），还原 P168a④ 具名 uchar 局部自然形（X2：`v ^= 1`）。
// 4.4 发 `movzbl; mov %al,-N; xorb $1,-N; cmpb $0,-N; je`（栈槽物化），ORIG 为
// `movzbl 0x380(%edx,%eax,1); xor $1,%eax; test %al,%al; je`（寄存器物化）⇒
// 自然形不可达，残差 = N96+N18a 封顶（P168a④ 补充）。
// 读数：清绑前 our_insns=163（TU 口径 AE）；清绑后 our_insns=164 / orig=163（TU 口径 DIFF）。
// ============================================================================
#include "CPartyDepsB2/die_user.inc"
}

// ============================================================================
// CParty::finish_loading —— ORIG 0x085b15e0
// ============================================================================
// [b28d-30] 复核 DIFF：帧/槽位跨版本差（N82/N93 族），ORIG 0x085b15e0。
// [N96 清绑 2026-09-09] 原 `register unsigned char fv asm("eax")` 全域禁用
// （P0②/N96/P248 尾条），还原 P168a④ 具名 uchar 局部自然形（X2：`v ^= 1`）。
// 4.4 发 `movzbl; mov %al,-N; xorb $1,-N; cmpb $0,-N; je`（栈槽物化），ORIG 为
// `movzbl 0xb10(%edx,%eax,1); xor $1,%eax; test %al,%al; je`（寄存器物化）⇒
// 自然形不可达，残差 = N96+N18a 封顶（P168a④ 补充）。
// 读数：清绑前 our_insns=96；清绑后 our_insns=97 / orig=86（TU 口径前后均 DIFF）。
#include "CPartyDepsB2/finish_loading.inc"
}

// ============================================================================
// CParty::checkDungeonDifficulty —— ORIG 0x085be932
// ============================================================================
#include "CPartyDepsB2/checkDungeonDifficulty.inc"
}

// ============================================================================
// CParty::_Send_TOD_APCInfo —— ORIG 0x085bac26（恒返回 1，单出口）
// ============================================================================
#include "CPartyDepsB2/_Send_TOD_APCInfo.inc"
}

// ============================================================================
// CParty::_onGetItem —— ORIG 0x085b949c
// ============================================================================
#include "CPartyDepsB2/_onGetItem.inc"
}

// ============================================================================
// CParty::choose_random_buff —— ORIG 0x0859b814（返回选中类型或 11）
// ============================================================================
#include "CPartyDepsB2/choose_random_buff.inc"
}

// ============================================================================
// CParty::checkInoutConditionDungeon —— ORIG 0x085abc80（三参重载，单出口）
// 返回错误码（0=通过）；错误路径同步向全队发包。
// ============================================================================
// [b28d-30] 复核 DIFF：巨件残差（N82/N89 族），ORIG 0x085abc80。
#include "CPartyDepsB2/checkInoutConditionDungeon.inc"
}

// ============================================================================
// CParty::giveup_game —— ORIG 0x085b2baa（单出口）
// 参数：user=放弃者 a2=允许惩罚 a3=按成功结算 a4=强制整队分支。
// ============================================================================
// [b28d-30] 复核 DIFF：巨件残差（N82/N89 族），ORIG 0x085b2baa。
#include "CPartyDepsB2/giveup_game.inc"
}
