// ORIG _ZN15cUserHistoryLog12StartRidableENS_12eRidableTypeEi @0x08686928：参数换位同 EndRidable
void cUserHistoryLog::StartRidable(eRidableType type, int a)
{
    m_user->LogHistory("Transform,%d,%d,%d", 1, a, (int)type);
}