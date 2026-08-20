// df_game_r Game/ WongWork::CHackAnalyzer 实现（独立 TU，从 GameStubs.cpp 拆出）。
#include "CUser.h"

namespace WongWork
{

void CHackAnalyzer::addServerHackCnt(CUser*, ENUM_HACKTYPE,
                                     unsigned int, unsigned int, unsigned int)
{
    // 报告确认该入口最终更新 CHackAnalyzer 的计数数组；当前头文件未
    // 暴露该数组/更新器，继续猜测会破坏真实布局，因此保留语义安全空体。
}

void CHackAnalyzer::resetHackInfo() {}
void CHackAnalyzer::resetServerHackAccumulatedCnt(ENUM_HACKTYPE) {}
void CHackAnalyzer::reportHackInfo() {}
void CHackAnalyzer::beginCollectHackInfo(CUser*) {}
void CHackAnalyzer::setLastMonsterDeadTime(long) {}

} // namespace WongWork
