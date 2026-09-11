int PvP_Room::get_remain_seat() const
{
    return 8 - get_waiter_count();
}