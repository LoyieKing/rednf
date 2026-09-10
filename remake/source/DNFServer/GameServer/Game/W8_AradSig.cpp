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

// ---------------------------------------------------------------------------
// SigServerState::tagData C1/C2 —— ORIG W 0x0818670c：
//   mov 0x8(%ebp),%eax; add $0x8,%eax; call AradServerStateMessage::Ctor(0x8186652)
// 即对成员 m_msg（tag 内 +0x08）执行 AradServerStateMessage 默认构造；
// C++ 中空体用户构造函数自动完成同样的成员对象构造。
// ---------------------------------------------------------------------------
#include "W8_AradSig/tagData.inc"

// ---------------------------------------------------------------------------
// SigAvatarRoulette —— ORIG W C1/C2 0x081a092e -> init() 0x081a0942：
//   movb $0x0,(%eax); movb $0x0,0x1(%eax); movb $0x0,0x2(%eax)
// （清 m_type / m_slotIdx / m_field02 三字节，含消费侧未读的第 3 字节）
// ---------------------------------------------------------------------------
#include "W8_AradSig/SigAvatarRoulette.inc"
{
    init();
}

#include "W8_AradSig/init.inc"
{
    m_type = 0;
    m_slotIdx = 0;
    m_field02 = 0;
}

// ---------------------------------------------------------------------------
// SigAradEventItemLog —— ORIG W C1/C2 0x8694f14 -> init() 0x8694f28：
//   movl $0x0,(%eax)[m_itemId]; movl $0x0,0x4(%eax)[m_itemCount]
// 仅清前两 dword（m_id/m_characNo 不初始化，与 ORIG 一致）。
// ---------------------------------------------------------------------------
#include "W8_AradSig/SigAradEventItemLog.inc"
{
    init();
}

#include "W8_AradSig/init_1.inc"
{
    m_itemId = 0;
    m_itemCount = 0;
}

// ---------------------------------------------------------------------------
// SigAradAvatarConvert —— ORIG W C1/C2 0x081a0a6a -> init() 0x081a0a7e：
//   movw $0x0,(%eax)[m_selectNo]; movw $0x0,0x2(%eax)[m_slot];
//   movl $0x0,0x4(%eax)[m_itemId]; movl $0x0,0x8(%eax)[m_resultItemId];
//   movw $0x0,0xc(%eax)[m_resultCount]
// ---------------------------------------------------------------------------
#include "W8_AradSig/SigAradAvatarConvert.inc"
{
    init();
}

#include "W8_AradSig/init_2.inc"
{
    m_selectNo = 0;
    m_slot = 0;
    m_itemId = 0;
    m_resultItemId = 0;
    m_resultCount = 0;
}

// ---------------------------------------------------------------------------
// SigAradEmblemCompound::materialInfo::init —— ORIG W 0x081a0a0c：
//   movw $0x0,(%eax)[slot]; movw $0x0,0x2(%eax)[grade]; movl $0x0,0x4(%eax)[itemIdx]
// ---------------------------------------------------------------------------
// P210：SigAradEmblemCompound::init 定义须先于本 callee，否则 4.4 按 materialInfo::init
// 帧精确继承 outgoing（caller 帧 0x18→0x4 与 ORIG 0x18 反）。
// ---------------------------------------------------------------------------
// SigAradEmblemCompound —— ORIG W C1/C2 0x081a0a2c -> init() 0x081a0a40：
//   sub $0x18（ORIG 保守 outgoing 帧）
//   movw $0x0,0x14(%eax)                        // 清 m_selectIdx
//   call materialInfo::init(this)               // 清 {m_slot0,m_grade0,m_itemId0}
//   eax += 0x8; call materialInfo::init(this+8) // 清 {m_slot1,m_grade1,m_itemId1}
// 注意 ORIG 不清 m_resultIdx(+0x10)。
// ---------------------------------------------------------------------------
#include "W8_AradSig/SigAradEmblemCompound.inc"
{
    init();
}

#include "W8_AradSig/init_3.inc"
{
    m_selectIdx = 0;
    reinterpret_cast<materialInfo*>(this)->init();
    reinterpret_cast<materialInfo*>((char*)this + 8)->init();
}

#include "W8_AradSig/init_4.inc"
{
    slot = 0;
    grade = 0;
    itemIdx = 0;
}

// ---------------------------------------------------------------------------
// SigLoadRewardEventItem —— ORIG W 全套：
//   C1/C2 0x08186686 -> init() 0x0818669a -> set(0) 0x081866b6；
//   set(v)：m_field0=v；m_count=0；memset(m_characNos,0,0x28)；
//           memset(m_eventCodes,0,0x28)
// （memset 先 characNos 区后 eventCodes 区，次序照 asm）
// ---------------------------------------------------------------------------
#include "W8_AradSig/SigLoadRewardEventItem.inc"
{
    init();
}

#include "W8_AradSig/set.inc"
{
    m_field0 = v;
    m_count = 0;
    memset(m_characNos, 0, sizeof(m_characNos));
    memset(m_eventCodes, 0, sizeof(m_eventCodes));
}

#include "W8_AradSig/init_5.inc"
{
    set(0);
}

// ---------------------------------------------------------------------------
// SigSaveRewardEventItem —— ORIG W C1/C2 0x08195cd4：调 init()（定义位于
// Arad_InternalDispatcher.cpp）；本 TU 仅提供 ctor。
// ---------------------------------------------------------------------------
#include "W8_AradSig/SigSaveRewardEventItem.inc"
{
    init();
}

// ---------------------------------------------------------------------------
// SigSetCharacCreateTime —— ORIG W：
//   C1/C2 0x0822ee3c -> init() 0x0822ee50 -> set(0,0) 0x0822ee74；
//   set(a,b)：+0=m_characNo=a、+4=m_createTime=b（两个 dword 直写）
// ---------------------------------------------------------------------------
#include "W8_AradSig/SigSetCharacCreateTime.inc"
{
    init();
}

// P210：init 定义先于 set（ORIG init 帧 0x18，4.4 若先见 set 定义则继承其小帧）。
#include "W8_AradSig/init_6.inc"
{
    set(0, 0);
}

#include "W8_AradSig/set_1.inc"
{
    m_characNo = characNo;
    m_createTime = createTime;
}

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
{
    init();
}

#include "W8_AradSig/init_7.inc"
{
    ((SigAradJumpingInitView*)this)->m_h0 = 0;
    ((SigAradJumpingInitView*)this)->m_head1 = 0;
    ((SigAradJumpingInitView*)this)->m_head5 = 0;
}

// ---------------------------------------------------------------------------
// SigAradUseAvatarRecharge —— ORIG W init() 0x081a09c4 / C1 0x081a09f8：
//   清 avatarSlot/stoneSlot/addEndurance/maxEndurance/enduranceAfter 五字段。
// ---------------------------------------------------------------------------
#include "W8_AradSig/SigAradUseAvatarRecharge.inc"
{
    init();
}

#include "W8_AradSig/init_8.inc"
{
    avatarSlot = 0;
    stoneSlot = 0;
    addEndurance = 0;
    maxEndurance = 0;
    enduranceAfter = 0;
}

// ---------------------------------------------------------------------------
// SigAradAvatarEnduranceSave —— ORIG W init() 0x081933de / C1 0x081933f6：
//   movl $0x0,(%eax)[uiId]; movl $0x0,0x4(%eax)[endurance]
// ---------------------------------------------------------------------------
#include "W8_AradSig/SigAradAvatarEnduranceSave.inc"
{
    init();
}

#include "W8_AradSig/init_9.inc"
{
    uiId = 0;
    endurance = 0;
}

// ---------------------------------------------------------------------------
// SigAvatarHiddenOptionChange —— ORIG W C1/C2 0x081a095c -> init() 0x081a0970：
//   清三个 word（m_materialSlot/m_avatarSlot/m_hiddenOption）
// ---------------------------------------------------------------------------
#include "W8_AradSig/SigAvatarHiddenOptionChange.inc"
{
    init();
}

#include "W8_AradSig/init_10.inc"
{
    m_materialSlot = 0;
    m_avatarSlot = 0;
    m_hiddenOption = 0;
}

// ---------------------------------------------------------------------------
// SigAradAvatarHiddenOptionSave —— ORIG W C1/C2 0x0817edfc -> init() 0x0817ee10：
//   清 m_field0/m_characNo/m_itId（dword×3）与 m_slot/m_hiddenOption（word×2）
// ---------------------------------------------------------------------------
#include "W8_AradSig/SigAradAvatarHiddenOptionSave.inc"
{
    init();
}

#include "W8_AradSig/init_11.inc"
{
    m_field0 = 0;
    m_characNo = 0;
    m_itId = 0;
    m_slot = 0;
    m_hiddenOption = 0;
}

}  // namespace arad
