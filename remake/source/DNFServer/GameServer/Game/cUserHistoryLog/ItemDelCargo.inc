void cUserHistoryLog::ItemDelCargo(int a, int b, int c, eItemDelReason reason)
{
    m_user->LogHistory("Inven-,%d,%d,%d,%d", a, b, c, static_cast<int>(reason));
}