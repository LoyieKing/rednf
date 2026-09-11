InstanceRentalSystem::InstanceRentalSystem()
    : m_impl(new RentalSystemImpl)
{
    // ORIG 0x08270014：new RentalSystemImpl(0x18) 交 auto_ptr；注册 PcRoom(1)/Dimension(2)。
    registerRentalMachine(1, new RentalMachinePcRoom);
    registerRentalMachine(2, new RentalMachineDimension);
}