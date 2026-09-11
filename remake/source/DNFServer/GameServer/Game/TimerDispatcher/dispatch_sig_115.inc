// [b28d-30] 复核 DIFF：push edi/esi/ebx 缓存组跨版本差（N7/N82 族），ORIG 0x08638xxx。
// [b28d-solo-TimerDispatcher] 判词 N7/N82附 封顶：getGradeCount 结果 ORIG 驻
// edi/ebx vs 4.4 槽化（-0x18/-0x14/-0x10/-0xc）+ 帧 0x6c↔leave（130/130）。
// [TimerDispatcher-2 2026-09-09 复核] gap=1 全为 callee-saved 角色：ORIG grade2/1/0
// 驻 edi/esi/ebx（push edi/esi/ebx + sub $0x6c）vs 4.4 全槽化（sub $0x88）⇒ N7/N82附；
// 裸 register 已试并回滚（130→133、帧 0x6c→0x7c）⇒ 封顶。
// [SweepTimerDispatcher2 2026-09-09] P329/P0 复核：ORIG 14 个 getGradeCount 结果
// **无任何具名局部**（前 4 个 spill 临时槽 -0x48/-0x44/-0x40/-0x3c，后 3 个驻
// edi/esi/ebx；makeRequest 侧 dbGrade6 驻 %esi、dbGrade1/0 驻 %edi/%ebx）⇒ 原码
// 全部内联到实参位（右→左求值序）。改为内联表达式后复验。
bool TimerUpdateCompoundEmblemStatics::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    if (getCompoundEmblemStatistic()->getTotalCount() != 0)
    {
        // [P227/P221] ORIG 在 cMyTrace ctor 之前右→左求值 7 个 getGradeCount
        // （前 4 个 spill -0x48/-0x44/-0x40/-0x3c，后 3 个驻 edi/esi/ebx）
        // ⇒ 必须写全表达式临时链（两语句形会把 ctor 提到求值之前）。
        cMyTrace(__PRETTY_FUNCTION__, 0x11aa, 0)(
            "DNF Compound Emblem Statistics (grade 0 : %d)(grade 1 : %d)(grade 2 : %d)(grade 3 : %d)(grade 4 : %d)(grade 5 : %d)(grade 6 : %d)",
            getCompoundEmblemStatistic()->getGradeCount(0),
            getCompoundEmblemStatistic()->getGradeCount(1),
            getCompoundEmblemStatistic()->getGradeCount(2),
            getCompoundEmblemStatistic()->getGradeCount(3),
            getCompoundEmblemStatistic()->getGradeCount(4),
            getCompoundEmblemStatistic()->getGradeCount(5),
            getCompoundEmblemStatistic()->getGradeCount(6));
        DB_UpdateCompoundEmblemStatics::makeRequest(
            // §4.3 具名化：+0x1b0 = m_serverEnvir(+0x198).m_channelNo(+0x18)
            G_CEnvironment()->m_serverEnvir.m_channelNo,
            getCompoundEmblemStatistic()->getGradeCount(0),
            getCompoundEmblemStatistic()->getGradeCount(1),
            getCompoundEmblemStatistic()->getGradeCount(2),
            getCompoundEmblemStatistic()->getGradeCount(3),
            getCompoundEmblemStatistic()->getGradeCount(4),
            getCompoundEmblemStatistic()->getGradeCount(5),
            getCompoundEmblemStatistic()->getGradeCount(6));
    }
    registNextTimer();
    getCompoundEmblemStatistic()->clear();
    return 1;
}