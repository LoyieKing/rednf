void Area::get_user_id_list(std::vector<unsigned short>& out)
{
    // [b28d-solo-Area] while+continue 真循环形（ORIG 086c305e）：user 预清(-0xc) +
    // P50 先声明后赋值 + P28 后置 it++（ppEi sret -0x18）+ id 具名槽(-0x12)。
    CUser* user = 0;
    std::map<unsigned short, CUser*>::iterator it;
    it = m_UsersInArea.begin();
    while (it != m_UsersInArea.end())
    {
        user = it->second;
        if (user == 0 || user->get_state() <= 2)
        {
            it++;
            continue;
        }
        // [b28d-solo-Area] ORIG 086c30fa `mov %ax,-0x12; lea -0x12; call
        // push_back(EOt)` ⇒ 实参是 get_unique_id() 的临时右值（无具名 id 局部）。
        out.push_back(user->get_unique_id());
        it++;
    }
