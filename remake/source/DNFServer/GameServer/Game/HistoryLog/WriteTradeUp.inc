void HistoryLog::WriteTradeUp(FILE* file, int itemIdx, int count)
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\306\256\267\271\300\314\265\345\040\276\367\270\256\275\272"
    "\306\256\040\111\104\072\045\144\040\103\157\165\156\164\072\045\144\015\012", time, itemIdx, count);
}