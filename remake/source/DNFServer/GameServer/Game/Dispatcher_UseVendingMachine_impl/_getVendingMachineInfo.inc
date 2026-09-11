// ORIG T 0x0821b698 _ZNK28Dispatcher_UseVendingMachine22_getVendingMachineInfoEPK18stVendingMachine_tj
//   在 vm->m_infoList 中按栏位 id（m_groupNum）匹配，返回栏信息（无则 0）。
// b28d-solo-Dispatcher_UseVendingMachine_impl [AE]：成员 vector 在 begin/end 处
//   内联重算（去具名引用，ORIG 0x0821b6a1 `lea 0x4(%eax),%edx` 两次）+ 迭代器循环。
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
}