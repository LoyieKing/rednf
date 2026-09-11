int WarRoom::GetRemainSeat() const
{
    return 6 - GetWaiterCount();
}