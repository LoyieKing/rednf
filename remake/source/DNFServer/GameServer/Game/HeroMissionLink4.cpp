// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 HeroMissionLink4/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// HeroMissionLink4.cpp — LinkMisc4 批次：事件/头像域 4 个 ORIG 符号。
//
// 逐报告忠实转写（docs/class_func_reports/）：
//   HeroMissionEvent::sendMissionData(CUser*)            T 0x0816770e sz 0x29d
//   HeroMissionEvent::setEventData(CUser*, char const*)  T 0x08166752 sz 0x5bc
//   EventClassify::CEventScriptMng::test_process_change_day(int)
//                                                        T 0x0810cc70 sz 0x13f
//   WongWork::CCompoundAvatar::ProcTest(ENUM_CHARACTERJOB, ENUM_EQUIPMENTTYPE,
//                                       int, int, std::map<int,int>&)
//                                                        T 0x08333fda sz 0x1e8
//
// HeroMissionEvent 为布局镜像类（HeroMissionEvent.h）：ORIG 开头的虚调用
// vptr+0x34 = CEventBase::IsEventing(this,0)（movzbl +0x08），此处按头内
// m_isEventing 标志等价判活。
// ============================================================================
#include "HeroMissionEvent.h"
#include "CCompoundAvatar.h"
#include "CDataManager.h"
#include "CEventScriptMng.h"
#include "CUser.h"
#include "GameWorld.h"
#include "GameRand.h"       // get_rand_int
#include "InterfacePacketBuf.h"
#include "LogManager.h"
#include "PacketGuard.h"
#include "PvPTypes.h"       // CMTRand 完整类型

#include <boost/tokenizer.hpp>

#include <map>
#include <string>
#include <utility>
#include <vector>

// ORIG _Z12getItemIndexRKSt6vectorISt4pairIiiESaIS1_EEi T 0x08332e4e sz 0x9a：
// 按 rand 值在 {itemId,count} 权重表中选 itemId。定义缺失（blocked，见交付），
// 此处仅声明以完成本批可编译。
#include "HeroMissionLink4/getItemIndex.inc"
#include "HeroMissionLink4/clear.inc"
}

// ---------------------------------------------------------------------------
// ORIG _ZN16HeroMissionValue5resetEv T 0x08164bb2（0x17，LinkFinal2 核销）：
// 仅 +0x04/+0x06 两个 ushort 清零（objdump 逐指令转写）。
// ---------------------------------------------------------------------------
#include "HeroMissionLink4/reset.inc"
}

// ---------------------------------------------------------------------------
// HeroMissionEvent::sendMissionData @ 0x0816770e
// ---------------------------------------------------------------------------
#include "HeroMissionLink4/sendMissionData.inc"
}


// ---------------------------------------------------------------------------
// EventClassify::CEventScriptMng::test_process_change_day @ 0x0810cc70
// ---------------------------------------------------------------------------
#include "HeroMissionLink4/test_process_change_day.inc"
}

// ---------------------------------------------------------------------------
// WongWork::CCompoundAvatar::ProcTest @ 0x08333fda
// ---------------------------------------------------------------------------
// [b28d-D33 P28/P61 封顶] ORIG 0x08333fda：142 vs 143 条；p167 差集 = ne/make_pair
// 非 const(MISSING) vs eq/make_pair const(EXTRA)（P61/P81 左值性 + P220 极性）。
// [F2d-C05② 甄别] 无命中形：gap=+1（show_func_diff orig=143 our=142，我们**少**）
//   ⇒ F2 减指令方向相反（残余 ORIG-only={mov×2} / OURS-only={add}）。维持封顶。
#include "HeroMissionLink4/ProcTest.inc"
}
// ---------------------------------------------------------------------------
// ORIG _Z12getItemIndexRKSt6vectorISt4pairIiiESaIS1_EEi T 0x08332e4e sz 0x9a：
// 顺序找首个 second 高于 rand 的表项返回其 first（itemId）；未命中返 0。
// ORIG 循环内 __gnu_cxxne 判续 + 循环后 __gnu_cxxeq 判 ==end 双实例化，
// 对应普通 for 迭代器循环。
// ---------------------------------------------------------------------------
// [b28d-D33 P28/N86 封顶] ORIG 0x08332e4e：56 vs 50 条；p167 差集 = vector::end
// EXTRA（P13 缓存 end 形）+ 迭代器槽（N86）。语义等价。
#include "HeroMissionLink4/getItemIndex_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG _ZN16HeroMissionValue14setStringTokenESs T 0x08164d68 sz 0x1aa：
//   clear 后按空格分隔（char_separator(" ", NULL, drop)）逐 token atoi 写入：
//   tok0→m_mission(int)、tok1→+0x04、tok2→+0x06、tok3→+0x0a、tok4→+0x08。
//   注意第 4/5 个 token 写入偏移与字段声明序相反（ORIG 如此，忠实保留）。
// ---------------------------------------------------------------------------
#include "HeroMissionLink4/setStringToken.inc"
}

// ---------------------------------------------------------------------------
// ~HeroMissionEvent（ORIG D1/D2 @0x081665a2 同体、D0 @0x0816662c）。
// 类现为真实继承（Singleton+CEventBase），虚析构由编译器自动发射
// D1/D2（同体）+ D0（call D1 + operator delete），与 ORIG 三形态对应。
// ORIG 析构顺序：m_values(+0x24) → m_conditions(+0x0c)，与本类成员声明逆序
// 一致（编译器按声明逆序自动析构）。
// ---------------------------------------------------------------------------
#include "HeroMissionLink4/dtor_HeroMissionEvent.inc"
}
