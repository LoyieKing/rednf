// ORIG T 0x0821b60a _ZNK28Dispatcher_UseVendingMachine18_getVendingMachineEj
//   遍历 G_CDataManager()->m_vendingMachine 按 m_id 匹配，返回机器指针（无则 0）。
// b28d-solo-Dispatcher_UseVendingMachine_impl [AE]：删除 dm==0 判空（ORIG 无），
//   改 `getVendingMachineTable()` + const_iterator 循环（ORIG 的 begin/end/
//   operator!=/operator++/operator* 调用点逐条对齐；ORIG 0x0821b610 段）。
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
}