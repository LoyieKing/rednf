void HistoryLog::WriteBuyItem(FILE* file, int itemIdx, int count)
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\276\306\300\314\305\333\261\270\300\324\040\111\104\072\040"
    "\045\144\054\040\103\157\165\156\164\072\045\144\015\012", time, itemIdx, count);
}