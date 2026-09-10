int TimerDeathTowerTopRankerReload::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    // ORIG 0x8637816：两阶段加载 TopRanker（false 增量 → true 全量），
    // 再把下轮触发排到次日 04:00 + rand(0..299) 分钟。
    DB_DeathTowerLoadTopRanker::makeRequest(false);
    DB_DeathTowerLoadTopRanker::makeRequest(true);
    registNextTimer(static_cast<long>(GlobalData::s_systemTime_.getCurSec()) + 0x15180);
    return 1;
