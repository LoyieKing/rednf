// df_game_r Game/ advancealtar::StageControl 专属 TU（从 GameStubs.cpp 拆出）。
// 布局依据 CGameManager.h（m_pad[0xc0]；ctor/dtor 由 CGameManager.cpp 提供）。
// getIndex/setIndex 依 ORIG 访问 +0 的 index；leaveUser/onTimerStageTick/reset
// 依赖 ProcStage/CharacData 完整布局（当前 CGameManager.h 仅为 pad 占位），
// 按任务约定保留为签名匹配的空体桩（符号 T），后续批次补全语义。
#include "CGameManager.h"

namespace advancealtar
{

// ORIG 0x81348ba：(const) return *(int*)this;
int StageControl::getIndex() const
{
    return *(const int*)(const void*)&m_pad[0];
}

// ORIG 0x82a669c：*(int*)this = idx;
void StageControl::setIndex(int idx)
{
    *(int*)(void*)&m_pad[0] = idx;
}

void StageControl::leaveUser()
{
}

void StageControl::onTimerStageTick()
{
}

void StageControl::reset()
{
}

}  // namespace advancealtar