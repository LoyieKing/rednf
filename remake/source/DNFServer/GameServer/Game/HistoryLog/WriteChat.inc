void HistoryLog::WriteChat(FILE* file, int type, char* content, int level)
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\303\244\306\303\305\270\300\324\072\045\144\040\263\273\277"
    "\353\072\040\045\163\015\012", time, type, content);
}