// ORIG 0x084b7932：若已受影响直接返回；按 beginSec 是否非零分派到活动/保留
// vector，并登记 TimerGlobalEffectItemBegin/Timeout。
void CGlobalEffectManager::registNewEffect(unsigned int type, float value,
                                           const char* name, long beginSec,
                                           long reserveSec)
{
    if (isAffectedEffect(type))
        return;

    if (beginSec == 0)
    {
        // ORIG 0x084b796a：getCurSec 结果直接 `add 0x1c(%ebp),%eax` 后落
        // 0x10(%esp) 实参槽，**无** endTime 具名槽（我方曾多 -0x10/-0xc 两槽
        // + 4 条 mov）；且 0x084b798c ctor 后 0x084b799e 立即 push_back，
        // 0x084b79c6 析构**先于** notify ⇒ 源形为临时量绑定 push_back(T&&)，
        // 非具名局部 + std::move（std::move 会多 lea/mov/call 三条/分支）。
        m_activeEffects.push_back(CGlobalEffect(
            type, value, name, GlobalData::s_systemTime_.getCurSec() + reserveSec));
        notifyGlobalEffectInfo();
        TimerGlobalEffectItemTimeout::registTimer(type, reserveSec);
    }
    else
    {
        m_reservedEffects.push_back(CGlobalEffect(
            type, value, name, GlobalData::s_systemTime_.getCurSec() + beginSec + reserveSec));
        TimerGlobalEffectItemBegin::registTimer(type, beginSec);
        TimerGlobalEffectItemTimeout::registTimer(type, beginSec + reserveSec);
    }
}