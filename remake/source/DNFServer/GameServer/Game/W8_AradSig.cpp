// ============================================================================
// W8_Mid.AradSigs —— arad 域 38 个仅-ORIG 符号（/tmp/only_orig/arad.txt）
// 对应类布局声明在 MoreTypes.h（namespace arad），实现转写自 df_game_r ORIG
// 反汇编；每个函数的 ORIG 地址见行内注释。全部具名字段访问。
// 需要登记 CMake 的文件：Game/W8_AradSig.cpp
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
SigServerState::tagData::tagData() {}

// ---------------------------------------------------------------------------
// SigAvatarRoulette —— ORIG W C1/C2 0x081a092e -> init() 0x081a0942：
//   movb $0x0,(%eax); movb $0x0,0x1(%eax); movb $0x0,0x2(%eax)
// （清 m_type / m_slotIdx / m_field02 三字节，含消费侧未读的第 3 字节）
// ---------------------------------------------------------------------------
SigAvatarRoulette::SigAvatarRoulette()
{
    init();
}

void SigAvatarRoulette::init()
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
SigAradEventItemLog::SigAradEventItemLog()
{
    init();
}

void SigAradEventItemLog::init()
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
SigAradAvatarConvert::SigAradAvatarConvert()
{
    init();
}

void SigAradAvatarConvert::init()
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
void SigAradEmblemCompound::materialInfo::init()
{
    slot = 0;
    grade = 0;
    itemIdx = 0;
}

// ---------------------------------------------------------------------------
// SigAradEmblemCompound —— ORIG W C1/C2 0x081a0a2c -> init() 0x081a0a40：
//   movw $0x0,0x14(%eax)                        // 清 m_selectIdx
//   call materialInfo::init(this)               // 清 {m_slot0,m_grade0,m_itemId0}
//   eax += 0x8; call materialInfo::init(this+8) // 清 {m_slot1,m_grade1,m_itemId1}
// 注意 ORIG 不清 m_resultIdx(+0x10)。
// ---------------------------------------------------------------------------
SigAradEmblemCompound::SigAradEmblemCompound()
{
    init();
}

void SigAradEmblemCompound::init()
{
    m_selectIdx = 0;
    reinterpret_cast<materialInfo*>(this)->init();
    reinterpret_cast<materialInfo*>((char*)this + 8)->init();
}

// ---------------------------------------------------------------------------
// SigLoadRewardEventItem —— ORIG W 全套：
//   C1/C2 0x08186686 -> init() 0x0818669a -> set(0) 0x081866b6；
//   set(v)：m_field0=v；m_count=0；memset(m_characNos,0,0x28)；
//           memset(m_eventCodes,0,0x28)
// （memset 先 characNos 区后 eventCodes 区，次序照 asm）
// ---------------------------------------------------------------------------
SigLoadRewardEventItem::SigLoadRewardEventItem()
{
    init();
}

void SigLoadRewardEventItem::set(unsigned int v)
{
    m_field0 = v;
    m_count = 0;
    memset(m_characNos, 0, sizeof(m_characNos));
    memset(m_eventCodes, 0, sizeof(m_eventCodes));
}

void SigLoadRewardEventItem::init()
{
    set(0);
}

// ---------------------------------------------------------------------------
// SigSaveRewardEventItem —— ORIG W C1/C2 0x08195cd4：调 init()（定义位于
// Arad_InternalDispatcher.cpp）；本 TU 仅提供 ctor。
// ---------------------------------------------------------------------------
SigSaveRewardEventItem::SigSaveRewardEventItem()
{
    init();
}

// ---------------------------------------------------------------------------
// SigSetCharacCreateTime —— ORIG W：
//   C1/C2 0x0822ee3c -> init() 0x0822ee50 -> set(0,0) 0x0822ee74；
//   set(a,b)：+0=m_characNo=a、+4=m_createTime=b（两个 dword 直写）
// ---------------------------------------------------------------------------
SigSetCharacCreateTime::SigSetCharacCreateTime()
{
    init();
}

void SigSetCharacCreateTime::set(unsigned int characNo, unsigned int createTime)
{
    m_characNo = characNo;
    m_createTime = createTime;
}

void SigSetCharacCreateTime::init()
{
    set(0, 0);
}

// ---------------------------------------------------------------------------
// SigAradJumpingCharacter —— ORIG W C1/C2 0x081a0990 -> init() 0x081a09a4：
//   movb $0x0,(%eax)          [m_head[0]]
//   movl $0x0,0x1(%eax)       [m_head[1..4]]
//   movl $0x0,0x5(%eax)       [m_head[5..8]]
// 仅清 m_head 九字节（memcpy 常量定长在 -O0 展开为同形态写入）；
// eventKind/growType/itemIndex/tail 不初始化。
// ---------------------------------------------------------------------------
SigAradJumpingCharacter::SigAradJumpingCharacter()
{
    init();
}

void SigAradJumpingCharacter::init()
{
    m_head[0] = 0;
    *reinterpret_cast<unsigned int*>(m_head + 1) = 0;
    *reinterpret_cast<unsigned int*>(m_head + 5) = 0;
}

// ---------------------------------------------------------------------------
// SigAradUseAvatarRecharge —— ORIG W init() 0x081a09c4 / C1 0x081a09f8：
//   清 avatarSlot/stoneSlot/addEndurance/maxEndurance/enduranceAfter 五字段。
// ---------------------------------------------------------------------------
SigAradUseAvatarRecharge::SigAradUseAvatarRecharge()
{
    init();
}

void SigAradUseAvatarRecharge::init()
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
SigAradAvatarEnduranceSave::SigAradAvatarEnduranceSave()
{
    init();
}

void SigAradAvatarEnduranceSave::init()
{
    uiId = 0;
    endurance = 0;
}

// ---------------------------------------------------------------------------
// SigAvatarHiddenOptionChange —— ORIG W C1/C2 0x081a095c -> init() 0x081a0970：
//   清三个 word（m_materialSlot/m_avatarSlot/m_hiddenOption）
// ---------------------------------------------------------------------------
SigAvatarHiddenOptionChange::SigAvatarHiddenOptionChange()
{
    init();
}

void SigAvatarHiddenOptionChange::init()
{
    m_materialSlot = 0;
    m_avatarSlot = 0;
    m_hiddenOption = 0;
}

// ---------------------------------------------------------------------------
// SigAradAvatarHiddenOptionSave —— ORIG W C1/C2 0x0817edfc -> init() 0x0817ee10：
//   清 m_field0/m_characNo/m_itId（dword×3）与 m_slot/m_hiddenOption（word×2）
// ---------------------------------------------------------------------------
SigAradAvatarHiddenOptionSave::SigAradAvatarHiddenOptionSave()
{
    init();
}

void SigAradAvatarHiddenOptionSave::init()
{
    m_field0 = 0;
    m_characNo = 0;
    m_itId = 0;
    m_slot = 0;
    m_hiddenOption = 0;
}

}  // namespace arad
