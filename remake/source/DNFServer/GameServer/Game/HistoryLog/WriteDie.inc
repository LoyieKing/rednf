void HistoryLog::WriteDie(FILE* file)
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\307\303\267\271\300\314\301\337\040\273\347\270\301\015\012", time);
}