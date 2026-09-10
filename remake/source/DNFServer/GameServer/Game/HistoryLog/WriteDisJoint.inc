void HistoryLog::WriteDisJoint(FILE* file, int itemIdx)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_DISJOINT, time, itemIdx);
