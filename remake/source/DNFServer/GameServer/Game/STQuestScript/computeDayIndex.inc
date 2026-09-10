int TrainingQuestScript::computeDayIndex()
{
    time_t now = time(0);
    struct tm* t = localtime(&now);
    return t->tm_yday;
