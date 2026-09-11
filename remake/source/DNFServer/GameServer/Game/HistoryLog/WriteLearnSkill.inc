void HistoryLog::WriteLearnSkill(FILE* file, int skillIdx, int level)
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\275\272\305\263\300\324\274\366\040\111\104\072\045\144\054"
    "\040\114\145\166\145\154\072\045\144\015\012", time, skillIdx, level);
}