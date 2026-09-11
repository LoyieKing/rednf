void HistoryLog::WriteKillMob(FILE* file, int mobType)
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\113\151\154\154\040\115\157\142\124\171\160\145\072\040\045"
    "\144\015\012", time, mobType);
}