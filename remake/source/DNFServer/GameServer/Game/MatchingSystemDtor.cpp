// ============================================================================
// [SplitC 2026-09-11 batch6 · A/B 类「拆壳即翻正」真增量件] 由 MatchingSystem.cpp 拆出（BScan v2 + SplitC 双口径确认真增量）。
//   件：_ZN15CMatchingSystemD1Ev；期望 verdict：AE；档位：$C6444R（= 444 后端 + 4.4.7 头集，即 444h47；444h46 同样 AE/ndiff0）
//   目的：独立编译单元化（消除同 TU 定义可见性/帧继承耦合）。
//   头集 = host 逐行复制（实测 GI=0）；.inc 原样搬移。
// ============================================================================
#include <cstring>
#include "CLeagueMatch.h"    // type 2：CLeagueMatch（new 0xfc）
#include "OnlinePreliminary.h"  // type 4：online_preliminary::COnlinePreliminary（new 0x70）
#include "CFairMatch.h"      // type 5：fair_pvp::CFairMatch（new 0xc4）

#include "MatchingSystemDtor/dtor_CMatchingSystem.inc"
