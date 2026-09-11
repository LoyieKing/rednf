int PvP_Room::check_acquire_guild_point()
{
    if (G_GameWorld()->IsPVPChannel())
    {
        if (G_GameWorld()->IsFreePvPChannel())
        {
            return 0;
        }
        if (!IsAllMoreThanGrade(10))
        {
            return 0;
        }
        if (IsThereSameGuildMember())
        {
            return 0;
        }
        return 1;
    }
    return 0;
}