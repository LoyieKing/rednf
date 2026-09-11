// ---- Dimension::checkRentalLevel —— T 0x0826f7e4（同名报告）----------------
bool RentalMachineDimension::checkRentalLevel(int level)
{
    if (0x40 < level)
    {
        return true;
    }
    return false;
}