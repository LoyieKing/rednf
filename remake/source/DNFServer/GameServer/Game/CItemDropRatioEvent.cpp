// df_game_r 物品掉落倍率事件 CItemDropRatioEvent 实现。
// 报告：docs/class_func_reports/CItemDropRatioEvent/ 全部 6 个方法
// （CItemDropRatioEvent / ~CItemDropRatioEvent / ~CItemDropRatioEvent_08533226 /
//  StartEvent / StartEvent_085332b6 / EndEvent）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN19CItemDropRatioEventC2Ev`（ORIG 0x85333f1）；ctor 已是首强符号。
#include <iostream>

#include "CItemDropRatioEvent.h"

#include "GameTypes.h"             // cMyTrace
#include "CDataManager.h"          // G_CDataManager / SetItemDropRate

// ---------------------------------------------------------------------------
// ORIG 0x085331d0：基类构造 + vptr 置位 + m_dropRate = 1。
CItemDropRatioEvent::CItemDropRatioEvent()
    : m_dropRate(1)
{
}

// ORIG 0x085331f6 / 0x08533226：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CItemDropRatioEvent::~CItemDropRatioEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x08533244：SetEventFlag(true) + m_dropRate = 1 +
// G_CDataManager()->SetItemDropRate(1.0f) +
// cMyTrace(__PRETTY_FUNCTION__,0x16,0)
// ("Item Drop Event Start!")。
void CItemDropRatioEvent::StartEvent()
{
    SetEventFlag(true);
    m_dropRate = 1;
    G_CDataManager()->SetItemDropRate(1.0f);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x16, 0);
    tr("Item Drop Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x085332b6：SetEventFlag(true) + m_dropRate = param.m_duration +
// G_CDataManager()->SetItemDropRate((float)m_dropRate) +
// cMyTrace(__PRETTY_FUNCTION__,0x1f,0)
// ("Item Drop Event Start!")。
void CItemDropRatioEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    m_dropRate = param.m_duration;
    G_CDataManager()->SetItemDropRate((float)m_dropRate);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1f, 0);
    tr("Item Drop Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x08533340：SetEventFlag(false) + m_dropRate = 1 +
// G_CDataManager()->SetItemDropRate(1.0f) +
// cMyTrace(__PRETTY_FUNCTION__,0x28,0)
// ("Item Drop Event End!")。
void CItemDropRatioEvent::EndEvent()
{
    SetEventFlag(false);
    m_dropRate = 1;
    G_CDataManager()->SetItemDropRate(1.0f);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x28, 0);
    tr("Item Drop Event End!");
}
