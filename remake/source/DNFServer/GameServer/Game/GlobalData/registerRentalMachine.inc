void InstanceRentalSystem::registerRentalMachine(int idx, IRentalMachine* m)
{
    m_impl->m_machines[idx] = m;
