// [K6 2026-09-11] int→bool（配合 DisPatcherCore.h:211）：ORIG 调用点 0x081ca1be
// `call; test %al,%al; je` 单字节判定（P292 档①）。ORIG 定义体 0x081ca2a8
// 尾 `mov $0x1,%eax`（32 位立即数）与 bool 返回在 4.1.2 下同形，故定义体不受影响。
bool DisPatcher_StartGame::excludeAddHackCount(CUser* user)

{
    if (user == 0)
    {
        return 0;
    }
    if (G_GameWorld()->isDungeonEntranceArea(user))
    {
        return 0;
    }
    if (user->getMoveSpace() == (UserSpace::T)1)
    {
        return 0;
    }
    return 1;
}
