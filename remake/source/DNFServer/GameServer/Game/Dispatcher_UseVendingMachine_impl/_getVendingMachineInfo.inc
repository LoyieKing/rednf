const stVendingMachineInfo_t* Dispatcher_UseVendingMachine::_getVendingMachineInfo(
    const stVendingMachine_t* vm, unsigned int slotId) const
{
    for (std::vector<stVendingMachineInfo_t>::const_iterator it = vm->m_infoList.begin(),
             e = vm->m_infoList.end(); it != e; ++it)
    {
        if (it->m_groupNum == slotId)
            return &(*it);
    }
    return 0;
