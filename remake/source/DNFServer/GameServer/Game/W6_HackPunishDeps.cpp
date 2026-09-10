// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 W6_HackPunishDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// Wave6 Hack 惩罚域实现（WongWork 黑名单 / IP 计数器 / 反作弊规则）。
// 控制流/常量取自 df_game_r 反汇编与
// docs/class_func_reports/WongWork__CAutoPunishRuleHackTypeMgr/ 报告。
// ============================================================================
#include "W6_HackPunishDeps.h"

#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

// ----------------------------------------------------------------------------
// 【临时】CUser.h 当前引用但尚未有任何权威头提供声明的类型（Wave6 进行中）；
// 仅本 TU 编译期可见：引用参数所需的最小前向 + 枚举占位（不带语义假设）。
// 权威头落地后删除本块并改 include。
// ----------------------------------------------------------------------------
#ifndef DNF_W6_HP_USER_FWD_TYPES
#define DNF_W6_HP_USER_FWD_TYPES
class CodeHackChecksum;
class CodeHackChecksumEx;
class CodeHackCheckRangeEx;
// [dedup] 两枚举权威定义见 CUser.h 守卫段（include 后即可见）。
#endif

#include "CUser.h"   // CUser::m_historyLog（+0x79700 cUserHistoryLog）

// Divide_C_ClassIP_EndIP 权威声明见 Divide_C_ClassIP_EndIP.h（W7 批次新建 TU）；
// ORIG 返回 void（_Z22Divide_C_ClassIP_EndIPPcS_S_ @0x086b1dd5），原 char* 声明修正。
#include "Divide_C_ClassIP_EndIP.h"

namespace WongWork
{

// ----------------------------------------------------------------------------
// HackIPCounter_t —— ORIG W C1/C2(EjjPcj) 0x080f9892 (0x63)：
// +0/+2 截 u16、+0x14=cnt，ip 区 memset(0xd)+strncpy(0xd)。C2 与 C1 同地址。
// ----------------------------------------------------------------------------
#include "W6_HackPunishDeps/HackIPCounter_t.inc"
}

// ----------------------------------------------------------------------------
// HackFullIPCounter_t —— ORIG W C1/C2(EjjPcj) 0x080f98f6 (0x63)：
// 同构，ip 区长 0x10。
// ----------------------------------------------------------------------------
#include "W6_HackPunishDeps/HackFullIPCounter_t.inc"
}

// ----------------------------------------------------------------------------
// CAutoPunishRuleBlackIPMgr::stIPInfo_t —— ORIG W C1/C2(Ehh) 0x084eb598 (0x27)。
// ----------------------------------------------------------------------------
#include "W6_HackPunishDeps/stIPInfo_t.inc"
}

// ----------------------------------------------------------------------------
// addBlackIP —— ORIG W 0x084eb5c0 (0x7e)：stIPInfo_t(a,b) 后 insert(make_pair(addr))，
// 固定 return true。
// ----------------------------------------------------------------------------
#include "W6_HackPunishDeps/addBlackIP.inc"
}

// ----------------------------------------------------------------------------
// isBlackIP —— ORIG W 0x084eb63e (0x18c)：把点分 IP 从末段向前找 '.'，
// 取末段数值 lastSeg，末段替换为 "0"，inet_addr 得网络地址 key；
// lower_bound..upper_bound(netAddr) 内区间 [minIp,maxIp] 覆盖 lastSeg 即命中。
// ----------------------------------------------------------------------------
#include "W6_HackPunishDeps/isBlackIP.inc"
}

// ----------------------------------------------------------------------------
// getBlackIPCount —— ORIG W 0x084eb7ca (0x13)：tail-call multimap::size()。
// ----------------------------------------------------------------------------
#include "W6_HackPunishDeps/getBlackIPCount.inc"
}

// ----------------------------------------------------------------------------
// setHackCnt —— ORIG W 0x084eb854 (0x17f)：HackTypeInfo_t(type1,u16(value1),
// type2,u16(value2),u16(v5),u16(v6),v7) 以 key=type1 insert；
// 若 type2 != 0 再以 key=type2 insert 第二条。
// ----------------------------------------------------------------------------
#include "W6_HackPunishDeps/setHackCnt.inc"
}

// ----------------------------------------------------------------------------
// setTotalTrade —— ORIG W 0x084eb9d4 (0x2c)：+0x18=type，+0x1c..=total(u64)。
// ----------------------------------------------------------------------------
#include "W6_HackPunishDeps/setTotalTrade.inc"
}

// ----------------------------------------------------------------------------
// GetExceptionCnt —— ORIG T 0x080f972a (0x8a)，sret 返回 POD(4B)：
// find(key)；命中取 pair.second，未命中 stHacktypeExceptionInfo_t(0,0)。
// ----------------------------------------------------------------------------
#include "W6_HackPunishDeps/GetExceptionCnt.inc"
}

// ----------------------------------------------------------------------------
// setIPCounterData —— ORIG T 0x080f9218 (0x109)：HackIPCounter_t(a,b,ip,cnt)
// 以 key=(u16)a insert 入 +0x24 表；b!=0 时再以 key=(u16)b 插一条新构造副本。
// ----------------------------------------------------------------------------
#include "W6_HackPunishDeps/setIPCounterData.inc"
}

// ----------------------------------------------------------------------------
// _findIPCount —— ORIG T 0x080f9322 (0x148)：equal_range((u16)a) 遍历，
// 元素 m_b==b 时 Divide_C_ClassIP_EndIP 取 class-C 前缀，
// 前缀非空且 strncmp(prefix, entry.m_ip, 0xc)==0 → 返回 m_cnt；否则 0。
// ----------------------------------------------------------------------------
#include "W6_HackPunishDeps/_findIPCount.inc"
}

// ----------------------------------------------------------------------------
// WriteIPCounterLog —— ORIG T 0x080f967a (0x57)：getUserP 非空时转发
// cUserHistoryLog(+0x79700)::IPCounterLog(...)。
// ----------------------------------------------------------------------------
#include "W6_HackPunishDeps/WriteIPCounterLog.inc"
}

// ----------------------------------------------------------------------------
// ResetIpCounterData —— ORIG W 0x084eba00 (0x16)：clear +0x24 multimap。
// ----------------------------------------------------------------------------
#include "W6_HackPunishDeps/ResetIpCounterData.inc"
}

// ----------------------------------------------------------------------------
// setFullIPCounterData —— ORIG T 0x080f946a (0x109)：对 +0x40 全 IP 表，
// 结构同 setIPCounterData。
// ----------------------------------------------------------------------------
#include "W6_HackPunishDeps/setFullIPCounterData.inc"
}

// ----------------------------------------------------------------------------
// _findFullIPCount —— ORIG T 0x080f9574 (0x105)：equal_range((u16)a) 遍历，
// 元素 m_b==b 且 strncmp(param3, entry.m_ip, 0x10)==0 → 返回 m_cnt；否则 0。
// ----------------------------------------------------------------------------
#include "W6_HackPunishDeps/_findFullIPCount.inc"
}

// ----------------------------------------------------------------------------
// WriteFullIPCounterLog —— ORIG T 0x080f96d2 (0x57)：转发
// cUserHistoryLog(+0x79700)::FullIPCounterLog(...)。
// ----------------------------------------------------------------------------
#include "W6_HackPunishDeps/WriteFullIPCounterLog.inc"
}

// ----------------------------------------------------------------------------
// ResetFullIpCounterData —— ORIG W 0x084eba16 (0x16)：clear +0x40 multimap。
// ----------------------------------------------------------------------------
#include "W6_HackPunishDeps/ResetFullIpCounterData.inc"
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// _checkApplyFlag —— ORIG W 0x0869550c。
// 依据 objdump 逐分支恢复：
//   m_value3 == 1/5  → 拷贝出参直接放行；
//   m_value3 == 2/10 → 在 lookOutsA/lookOutsB 观察名单中命中 {m_type1,m_type2}
//                      即拒绝；m_value3 == 3 → 在 ipCounters 名单中命中即拒绝；
//   m_value3 == 4/9  → _findIPCount 超阈值记日志并拒绝，否则记日志继续；
//   m_value3 == 0xb/0xc → _findFullIPCount 同型处理；
//   其余 → 拷贝出参放行。
// ----------------------------------------------------------------------------
#include "W6_HackPunishDeps/_checkApplyFlag.inc"
}

// ----------------------------------------------------------------------------
// checkMaxHackCntOver —— ORIG W 0x08695360。
// 遍历 m_hackTypeMap 中 key == type 的 [lower_bound, upper_bound) 区间；
// 规则条目按两个对称条件判断"累计计数超限"（条件成立即置 flag）：
//   A: m_type1==type && m_value1<=param && getServerHackAccumulatedCnt(m_type2)>=m_value2
//   B: m_type2==type && m_value2<=param && getServerHackAccumulatedCnt(m_type1)>=m_value1
// flag 置位后交由 _checkApplyFlag 处置；返回 true 表示命中处置（调用方终止）。
// ----------------------------------------------------------------------------
#include "W6_HackPunishDeps/checkMaxHackCntOver.inc"
}

// checkTotalTradeRuleOver —— ORIG W 0x08531fd2 (0x63) const：
//  type==0 且 totalValue==0          -> false
//  totalTradeType > tradeType        -> false
//  value 低 32 位 > total 低 32 位    -> false
//  低相等时高位 > total 高位          -> false
//  其余                               -> true
// （即记录的总量阈值未被当前 total 超越即视为 over。）
// ----------------------------------------------------------------------------
#include "W6_HackPunishDeps/checkTotalTradeRuleOver.inc"
}

}  // namespace WongWork
