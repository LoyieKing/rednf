void HistoryLog::starAdd(CUser& user, AddStarType::T type, int stageIndex,
                         int count)
{
    user.LogHistory("star+,%d,%d,%d", static_cast<int>(type), stageIndex,
                    count);
    HistoryLog::advanceAltarstatistics_.incrementEntranceData(
        static_cast<unsigned int>(stageIndex), 0, 0,
        static_cast<unsigned int>(count));
