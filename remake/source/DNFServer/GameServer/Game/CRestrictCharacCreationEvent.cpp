// df_game_r 限制创建角色事件 CRestrictCharacCreationEvent 实现。
// 报告：docs/class_func_reports/CRestrictCharacCreationEvent/ 全部 7 个方法
// （CRestrictCharacCreationEvent / ~CRestrictCharacCreationEvent /
//  ~CRestrictCharacCreationEvent_08273c9a / StartEvent / StartEvent_08273d0a /
//  EndEvent / getLimitCreateCharacter）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN28CRestrictCharacCreationEventC2Ev`（ORIG 0x8273e2a）；ctor 已是首强符号。
#include <iostream>

#include "CRestrictCharacCreationEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x08273c44：基类构造 + vptr 置位 + m_limitType(+0x0c) 置 2。
CRestrictCharacCreationEvent::CRestrictCharacCreationEvent()
{
    m_limitType = 2;
}

// ORIG 0x08273c6a / 0x08273c9a：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CRestrictCharacCreationEvent::~CRestrictCharacCreationEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x08273cb8：SetEventFlag(true) +
// cMyTrace("virtual void CRestrictCharacCreationEvent::StartEvent()",0x17,0)
// ("Restrict Character Creation Start!")。
void CRestrictCharacCreationEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x17, 0);
    tr("Restrict Character Creation Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x08273d0a：SetEventFlag(true) → m_limitType = param 低 ushort
// （movzwl 0xc(%ebp) → mov %edx,0xc(%eax) 整 4 字节写）→ trace（行 0x23）。
void CRestrictCharacCreationEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    m_limitType = param.m_duration;
    cMyTrace tr(__FUNCTION__, 0x23, 0);
    tr("Restrict Character Creation Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x08273d68：SetEventFlag(false) +
// cMyTrace("virtual void CRestrictCharacCreationEvent::EndEvent()",0x2b,0)
// ("Restrict Character Creation End!")。
void CRestrictCharacCreationEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x2b, 0);
    tr("Restrict Character Creation End!");
}

// 1 → 限 8；其它 → 0。ORIG 汇编先 test/cmp/jmp 线性分派且 m_limitType 只读
// 一次（mov 0xc(%eax) → test → cmp $1 → jmp else）⇒ switch 决策树形态
// （P46：稀疏 switch 线性 cmp 链，叶块按 case 升序）。
int CRestrictCharacCreationEvent::getLimitCreateCharacter(int characCount) const
{
    switch (m_limitType)
    {
    case 0:
        if (characCount == 0)
        {
            return 7;
        }
        break;
    case 1:
        return 8;
    }
    return 0;
}
