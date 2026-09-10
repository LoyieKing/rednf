const stVendingMachine_t* Dispatcher_UseVendingMachine::_getVendingMachine(
    unsigned int machineId) const
{
    const std::vector<stVendingMachine_t>& table =
        *G_CDataManager()->getVendingMachineTable();
    for (std::vector<stVendingMachine_t>::const_iterator it = table.begin(),
             e = table.end(); it != e; ++it)
    {
        if (it->m_id == machineId)
            return &(*it);
    }
    return 0;
