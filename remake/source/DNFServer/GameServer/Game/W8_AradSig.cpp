// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W8_AradSig/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "MoreTypes.h"

#include <cstring>

namespace arad {

#include "W8_AradSig/tagData.inc"

#include "W8_AradSig/SigAvatarRoulette.inc"


#include "W8_AradSig/init.inc"


#include "W8_AradSig/SigAradEventItemLog.inc"


#include "W8_AradSig/init_1.inc"


#include "W8_AradSig/SigAradAvatarConvert.inc"


#include "W8_AradSig/init_2.inc"


#include "W8_AradSig/SigAradEmblemCompound.inc"


#include "W8_AradSig/init_3.inc"


#include "W8_AradSig/init_4.inc"


#include "W8_AradSig/SigLoadRewardEventItem.inc"


#include "W8_AradSig/set.inc"


#include "W8_AradSig/init_5.inc"


#include "W8_AradSig/SigSaveRewardEventItem.inc"


#include "W8_AradSig/SigSetCharacCreateTime.inc"


#include "W8_AradSig/init_6.inc"


#include "W8_AradSig/set_1.inc"


// ---------------------------------------------------------------------------
// SigAradJumpingCharacter —— ORIG W C1/C2 0x081a0990 -> init() 0x081a09a4：
//   movb $0x0,(%eax)          [m_head[0]]
//   movl $0x0,0x1(%eax)       [m_head[1..4]]
//   movl $0x0,0x5(%eax)       [m_head[5..8]]
// 仅清 m_head 九字节（memcpy 常量定长在 -O0 展开为同形态写入）；
// eventKind/growType/itemIndex/tail 不初始化。
// ---------------------------------------------------------------------------
#pragma pack(push, 1)
// b28：fY 探针（/tmp/bl14_p3.o）实证 pack(1) 具名字段视图直赋复现 ORIG `movl $0x0,0x1(%eax)`
// disp 直存形；`*(unsigned*)(m_head+K)=0` 指针算术形 4.4 恒拆 add+movl（+2 条）。
struct SigAradJumpingInitView
{
    char m_h0;
    unsigned int m_head1;
    unsigned int m_head5;
};
#pragma pack(pop)

#include "W8_AradSig/SigAradJumpingCharacter.inc"


#include "W8_AradSig/init_7.inc"


#include "W8_AradSig/SigAradUseAvatarRecharge.inc"


#include "W8_AradSig/init_8.inc"


#include "W8_AradSig/SigAradAvatarEnduranceSave.inc"


#include "W8_AradSig/init_9.inc"


#include "W8_AradSig/SigAvatarHiddenOptionChange.inc"


#include "W8_AradSig/init_10.inc"


#include "W8_AradSig/SigAradAvatarHiddenOptionSave.inc"


#include "W8_AradSig/init_11.inc"


}  // namespace arad
