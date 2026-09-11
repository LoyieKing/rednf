// [b28d-7 N95/N82] ORIG 0x085d88ae：栈上对象槽 -0x2c/-0x30，4.4 -0x20/-0x24（槽基差 0xc）+ 帧 0x60 vs 0x50；P167 CLEAR ⇒ 封顶。
// [b28d-solo-PvP_Room] N95-N82: ORIG 0x085d88ae 栈上对象槽 -0x2c/-0x30 vs 4.4 -0x20/-0x24
int PvP_Room::DrawRandomMapIndex(std::vector<int>& out)
{
    bool local_21 = false;
    std::vector<int> local_30;
    std::vector<int>::iterator it = out.begin();
    while (it != out.end())
    {
        if (IsDeathMatchMap(*it, local_21) != 0)
        {
            local_30.push_back(*it);
            it = out.erase(it);
        }
        else
        {
            ++it;
        }
    }
    int local_1c = 0;
    if ((local_30.size() == 0) && (out.size() == 0))
    {
        local_1c = rand() % G_CDataManager()->m_mapList->GetPVPMapCount() + 1;
    }
    else if ((local_30.size() == 0) && (out.size() != 0))
    {
        // [b28d-solo-PvP_Room] P0：ORIG 0x085d88ae 对下标先落具名槽
        // （`mov %eax,-0x14(%ebp); mov -0x14(%ebp),%eax; mov %eax,0x4(%esp)`）
        // ⇒ 原码有具名 int 索引局部，非内联 `out[rand()%size()]`。
        int idx = rand() % out.size();
        local_1c = out[idx];
    }
    else if ((out.size() == 0) && (local_30.size() != 0))
    {
        int idx = rand() % local_30.size();
        local_1c = local_30[idx];
    }
    else
    {
        int idx = rand() % out.size();
        local_1c = out[idx];
    }
    return local_1c;
}