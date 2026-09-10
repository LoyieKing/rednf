// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 UserPacketCounter/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// UserPacketCounter.cpp —— message_counter::CUserPacketCounter 实现
//   + cUserHistoryLog::AbnormalUser（ORIG 0x08686df4，声明在 CUser.h）。
// 全部函数体按 ORIG df_game_r objdump 逐指令转写。
// ============================================================================

#include "UserPacketCounter.h"
#include "CMyFileLog.h"
#include "CUser.h"       // cUserHistoryLog 权威声明 / CUser::LogHistory

#include <stdio.h>
#include <utility>       // std::make_pair

// ORIG 自由函数（权威声明 CInventory.h:408；此处按项目惯例局部声明，签名一致）
char* NumberToString(unsigned int value, int radix);

namespace message_counter
{

// ORIG W 0x0847e8d2：map 默认构造（编译器生成）+ total=0 +
// sprintf(m_logPath, "./log/%s/%s", dir, name)。
#include "UserPacketCounter/CUserPacketCounter.inc"
}

// ORIG W 0x0847e916：仅 map 析构（其余成员平凡）。
#include "UserPacketCounter/dtor_CUserPacketCounter.inc"
}

// ORIG T 0x0856ff24：
//   find(accId) != end → ++it->second；否则 insert(make_pair(accId, 1))；
//   最后 ++m_totalCount（+0x118）。
#include "UserPacketCounter/IncreasePacket.inc"
}

// ORIG T 0x0856fff0：
//   CMyFileLog(m_logPath, 0x9d)(m_logPath, "user_packet_counter total_count(%d)", total)
//   遍历 map：idStr = NumberToString(key, 0)；
//     CMyFileLog(m_logPath, 0xa0)(m_logPath, "(%s,%12d)", idStr, value)
//   收尾：total=0、map.clear()。
// （ORIG 每条记录都新建一个 CMyFileLog，忠实保留。）
#include "UserPacketCounter/WriteLog.inc"
}

}  // namespace message_counter

// ----------------------------------------------------------------------------
// ORIG T 0x08686df4 (_ZN15cUserHistoryLog12AbnormalUserE27ENUM_ABNORMAL_USER_LOG_TYPEtii)：
//   eax = *(this)（+0x00 m_user，无空检查）；以
//   m_user->LogHistory("AbnormalUser,%u,%hu,%d,%d", type, value, a3, a4) 转发。
// 格式串 ORIG @0x8cf1037 已核对。
// ----------------------------------------------------------------------------
#include "UserPacketCounter/AbnormalUser.inc"
}
