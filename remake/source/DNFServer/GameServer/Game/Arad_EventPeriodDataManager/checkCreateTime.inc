bool Arad_EventPeriodDataManager::checkCreateTime(
    ENUM_REPEAT_EVENT_CODE eventCode, unsigned int time, unsigned int gmId)
{
    SearchEventPeriod pred(eventCode);
    std::vector<SCRIPT::SEventPeriodData>::iterator it =
        std::find_if(m_eventPeriodList.begin(), m_eventPeriodList.end(), pred);
    // P167：ORIG 生成 operator!=
    if (!(it != m_eventPeriodList.end()))
    {
        return false;
    }

    // [L23-r2 判据⑫ widthscan] `clr_b orig=1 our=0`：ORIG 有 `movb $0x0,-N(%ebp)`
    //   预清（call 前）⇒ P198① 两句式「声明带初值 + 独立赋值」；合写会丢预清行，
    //   且该 bool 局部同时是槽集合差 4 的来源（我方槽整体浅 4）。
    bool isGM = false;
    isGM = GlobalData::s_pGMAccounts_->isGM(gmId);
    unsigned int beginTime = 0x5005D250u;
    if (isGM)
    {
        beginTime += (unsigned int)(m_field8 * -86400);
    }
    return (beginTime <= time) && (time < it->m_endTime);
