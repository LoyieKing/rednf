// ORIG T 0x08133db0（starSub.md 全文）：仅 LogHistory("star-,%d,%d")。
void HistoryLog::starSub(CUser& user, SubStarType::T type, int value)
{
    user.LogHistory("star-,%d,%d", static_cast<int>(type), value);
}