// ORIG 0x08286462
//   _ZN24CHackLog_InvalidAreaMove11IsSeriaRoomEii
//   任一 (a,b) 匹配即返回 1：
//   (1,1) (2,5) (3,2) (4,1) (5,1) (6,4) (9,2) (11,3)
bool CHackLog_InvalidAreaMove::IsSeriaRoom(int a, int b)
{
    // ORIG 0x08286462 岛形：先 (1,1) 独立 return 岛（mov$1;jmp 汇入），其后主链
    // 仍以 (a==1&&b==1) 为第一项；4.4 若只给原链会折掉先导岛，须显式先判一次。
    if (a == 1 && b == 1)
        return 1;
    if ((a == 1 && b == 1) ||
        (a == 2 && b == 5) ||
        (a == 3 && b == 2) ||
        (a == 4 && b == 1) ||
        (a == 5 && b == 1) ||
        (a == 6 && b == 4) ||
        (a == 9 && b == 2) ||
        (a == 11 && b == 3))
        return 1;
    return 0;
}