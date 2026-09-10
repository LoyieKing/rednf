std::vector<stVendingMachine_t>* CDataManager::getVendingMachineTable() const
{
    return (std::vector<stVendingMachine_t>*)&m_vendingMachine;
