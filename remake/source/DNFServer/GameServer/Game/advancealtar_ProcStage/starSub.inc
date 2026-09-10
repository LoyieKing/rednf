void HistoryLog::starSub(CUser& user, SubStarType::T type, int value)
{
    user.LogHistory("star-,%d,%d", static_cast<int>(type), value);
