// ORIG _ZN15cUserHistoryLog27SelectCubeForPremiumServiceEi @0x08686da0
void cUserHistoryLog::SelectCubeForPremiumService(int a)
{
    if (m_user != 0)
        m_user->LogHistory("SelectCubeForPremiumService,%d", a);
    return;
}