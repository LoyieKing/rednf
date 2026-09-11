// ORIG W 0x0822b64c：return this+0xb424（&m_vendingMachine）。
std::vector<stVendingMachine_t>* CDataManager::getVendingMachineTable() const
{
    return (std::vector<stVendingMachine_t>*)&m_vendingMachine;
}