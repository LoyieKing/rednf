// ORIG _ZN15cUserHistoryLog10EndRidableENS_12eRidableTypeEi @0x0868695c：注意参数换位
void cUserHistoryLog::EndRidable(eRidableType type, int a)
{
    m_user->LogHistory("Transform,%d,%d,%d", 0, a, (int)type);
}