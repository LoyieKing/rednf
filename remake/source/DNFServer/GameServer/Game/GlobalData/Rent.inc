// Rent 分发（ORIG T 0x08270288）：按 RentInput.m_machineKey 取机器并经槽2
// 调用；无机器时 ORIG 直接返回 1。
// [b28 判词 N7] 残差=call *%edx↔%eax 换名+实参链寄存对调，29 行主体同形封顶。
int InstanceRentalSystem::Rent(CUser& user, const RentInput& input,
                               RentOutput& output)
{
    IRentalMachine* machine = getMachine(input.m_machineKey);
    if (machine == 0)
    {
        return true;
    }
    return machine->Rent(user, input, output);
}