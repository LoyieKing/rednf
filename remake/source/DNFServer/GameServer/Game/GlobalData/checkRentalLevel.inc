bool RentalMachineDimension::checkRentalLevel(int level)
{
    if (0x40 < level)
    {
        return true;
    }
    return false;
