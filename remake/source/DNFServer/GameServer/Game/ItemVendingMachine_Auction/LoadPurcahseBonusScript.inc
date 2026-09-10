bool ItemVendingMachine::LoadPurcahseBonusScript()
{
    if (!m_bonus.LoadScript()) return false;
    return true;
