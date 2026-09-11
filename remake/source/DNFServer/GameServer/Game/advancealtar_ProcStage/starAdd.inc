// ----------------------------------------------------------------------------
// ORIG T 0x08133d56（starAdd.md 全文）：LogHistory("star+,%d,%d,%d")+
// incrementEntranceData(advanceAltarstatistics_, idx,0,0,cnt)。
// ----------------------------------------------------------------------------
void HistoryLog::starAdd(CUser& user, AddStarType::T type, int stageIndex,
                         int count)
{
    user.LogHistory("star+,%d,%d,%d", static_cast<int>(type), stageIndex,
                    count);
    HistoryLog::advanceAltarstatistics_.incrementEntranceData(
        static_cast<unsigned int>(stageIndex), 0, 0,
        static_cast<unsigned int>(count));
}