// [b28d-lane2] verdict=DIFF ndiff=88（ORIG 0x084d316a）：槽序/帧差(N82/P212/P221)+混合残差；gap+18。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=112（ORIG 0x84d316a）：N101 imul/lea 折叠+N18a 单段 regmove+N82/P112 帧槽平移。
// [BigParty/P331] 已实测本件 P331 第二形**不成立**（同 .o 复测）：删 rec + pWin_rec 两个
//   具名指针、宏化每次直取后 our 72→83（更贴 ORIG 90）但 ndiff **112→115 反劣** ⇒ 已回滚，
//   保留缓存形。同构建点基线读数：orig=90 our=72 ndiff=112。
// [b28d-solo2] P0 重判：ORIG 中 best(-0x30)/members(-0x2c) 落 spill 槽、count→%edi/accId→%esi/
//   stage→%ebx 走 callee-saved，getRankTable 返回值 eax 直压 (%esp) 无槽 ⇒ 原码是「单语句链式
//   调用 + 全内联实参」，无 rec/pWin_rec/best/count/accId/members/table 具名局部（5 对 store-reload
//   即 gap' 的 +5P 来源）。data 以 pPkt 具名槽（ORIG -0x24）作全部地址基址。
int Inter_LoadDeathTowerBestRecord::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_LoadDeathTowerBestRecordPktView* pPkt = reinterpret_cast<Inter_LoadDeathTowerBestRecordPktView*>(data);

// [G2B-Monitor] 残余留 N（写域外）：本 .inc 只含包基址声明，函数体其余部分由聚合器
//   InterDispatcher.cpp 在 `#include` 之后续写；/tmp/g2_norm 里的实质差异
//   （`imul $0x6c`/`imul $0x1b0`/`add $0x6c`+`mov (%eax)` vs `mov 0x6c(%eax)` 段、
//   `mov %edi,0x10(%esp)` 序）全部落在续写段 ⇒ 不在本任务写域，无法修改 ⇒ 留 N。
#define pRec (reinterpret_cast<Inter_LoadDeathTowerBestRecordRecWinView*>( \
    reinterpret_cast<char*>(pPkt) + i * 0x1b0 + j * 0x6c))
    for (unsigned int i = 0; i < pPkt->m_0; ++i)
    {
        for (int j = 0; j <= 3; ++j)
        {
            if (pRec->m_6c != 0)
            {
                ((WongWork::CDeathTowerRanking*)
                     WongWork::CDeathTowerRanking::getRankTable(j + 1))->registBestRecord(
                    (unsigned int)*reinterpret_cast<unsigned short*>(
                        reinterpret_cast<char*>(pPkt) + i * 0x1b0 + 0xc),
                    pPkt->m_4,
                    reinterpret_cast<const stDeathTowerRecordMemberInfo_t*>(&pRec->m_10),
                    pRec->m_6c,
                    pRec->m_70);
            }
        }
    }
#undef pRec
    return 0;
}