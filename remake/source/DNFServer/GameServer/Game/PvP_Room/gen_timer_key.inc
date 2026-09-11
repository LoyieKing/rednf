unsigned int PvP_Room::gen_timer_key(TIMER_MESSAGE msg)
{
    return ++P_TIMER_KEY(msg);
}