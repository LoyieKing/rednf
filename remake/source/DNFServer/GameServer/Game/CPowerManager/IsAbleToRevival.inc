bool CPowerManager::IsAbleToRevival(CUser* user, unsigned int charNo, int& result)
{
    // ORIG 0x0847f952：复活冷却判定。penalty 同 GetPowerWarResponPenalty，
    // ghost=ROUND(ghostTime*penalty)，与 (charNo - 混沌死亡时刻) 比较。
    // [b28d-28] ORIG 0x0847f98d：unsigned int→float 走 64 位零扩展
    // （`mov $0,%edx; mov %eax,-0x28; mov %edx,-0x24; fildll`，同
    // GetPowerWarResponPenalty 0x08480f03）；尾块 ORIG 为
    // `jle L_then`（ghost<=delta → result=0;return 1 在后）、
    // ghost>delta 直落（result=ghost-delta; return 0）⇒ 源码取正向早返形。
    float penalty = 1.0f;
    if ((unsigned int)user->GetCurCharacChaosStateTime() <
        (unsigned int)G_CDataManager()->m_powerParameter.m_penaltyThreshold)
    {
        penalty = 2.0f -
                  (float)(unsigned long long)(unsigned int)user->GetCurCharacChaosStateTime() /
                      (float)G_CDataManager()->m_powerParameter.m_penaltyThreshold;
    }
    int ghost = (int)((float)G_CDataManager()->m_powerParameter.m_ghostTime * penalty);
    // [b28d-28] ORIG 0x0847f9f2 装载序：`call GetCurCharacChaosDieTime` 之后才
    // `mov 0x10(%ebp),%edx`（charNo）⇒ 无符号中间量 d = charNo - (unsigned)die
    // 使 4.4 求值序翻为「先 call 后读 charNo」（探针 /tmp/l28_probe/p3.cpp g4 全等）。
    int delta = (int)(charNo - (unsigned int)user->GetCurCharacChaosDieTime());
    if (ghost > delta)
    {
        result = ghost - delta;
        return false;
    }
    result = 0;
    return true;
