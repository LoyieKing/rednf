void HistoryLog::WriteUseCoin(FILE* file, const char* reason)
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\304\332\300\316\273\347\277\353\040\055\076\040\045\163\015"
    "\012", time, reason);
}