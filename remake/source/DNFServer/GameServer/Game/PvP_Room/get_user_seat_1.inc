int PvP_Room::get_user_seat(int seat)
{
    // ORIG 085d921e：无帧；`mov $0;jmp pop` 块物理在前、body 尾块 fall 直 ret。
    // 越界或形守卫（js/jle 双入 return0 块）复现块序。
    if ((seat < 0) || (seat > 7))
        return 0;
    return (int)P_SEATS(seat);
}