int TimerUpdateCompoundEmblemStatics::dispatch_sig(int p1, int p2, unsigned int p3)
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
