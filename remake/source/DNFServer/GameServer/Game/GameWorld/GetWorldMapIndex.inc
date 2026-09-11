int GameWorld::GetWorldMapIndex(CUser* user)
{
    // ORIG 0x86cebe6-0x86cebf1：null 检查产 `jne body` + fall-through `mov$-1;jmp end`
    // 独立早退岛（body 内 3 处失败共用尾岛 0x86cec7d）⇒ 源码为 early-return 形
    if (user == NULL)
    {
        return -1;
    }
    {
        int vill = (int)user->getCurCharacVill();
        int area = user->get_area(false);
        if (check_valid_area(vill, area))
        {
            Village* village = getUserVillage(user);
            if (village != NULL)
            {
                Area* pArea = village->getArea(area);
                if (pArea != NULL)
                {
                    return pArea->get_world_map_index();
                }
            }
        }
    }
    return -1;
}