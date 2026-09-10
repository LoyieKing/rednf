bool InstanceRentalSystem::CheckRentableItem(int type, unsigned int itemIdx)
{
    IRentalMachine* machine = getMachine(type);
    if (machine == 0)
        return true;
    return machine->CheckRentableItem(itemIdx);
