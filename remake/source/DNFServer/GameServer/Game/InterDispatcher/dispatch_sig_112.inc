int Inter_LoadDeathTowerBestRecord::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_LoadDeathTowerBestRecordPktView* pPkt = reinterpret_cast<Inter_LoadDeathTowerBestRecordPktView*>(data);

// [G2B-Monitor] 残余留 N（写域外）：本 .inc 只含包基址声明，函数体其余部分由聚合器
//   InterDispatcher.cpp 在 `#include` 之后续写；/tmp/g2_norm 里的实质差异
//   （`imul $0x6c`/`imul $0x1b0`/`add $0x6c`+`mov (%eax)` vs `mov 0x6c(%eax)` 段、
//   `mov %edi,0x10(%esp)` 序）全部落在续写段 ⇒ 不在本任务写域，无法修改 ⇒ 留 N。
