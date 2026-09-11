void HistoryLog::WriteFineQuest(FILE* file, int questIdx)
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\304\371\275\272\306\256\040\277\317\267\341\072\040\045\144"
    "\015\012", time, questIdx);
}