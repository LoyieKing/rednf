void cUserHistoryLog::apply_Item(int a, int b, eApplyItemReason reason)
{
    if (m_user)
        m_user->LogHistory("ApplyItem ,%d,%d,%d", a, b, static_cast<int>(reason));
    return;
