// ---------------------------------------------------------------------------
// CUser::GetUserListByParty(std::vector<CUser*>& out) T 0x0868056a
// push self；有队伍时按槽 0..3 补齐他人（跳过空/自身），返回队伍指针。
// ---------------------------------------------------------------------------
CParty* CUser::GetUserListByParty(std::vector<CUser*>& out)
{
    out.push_back(this);
    CParty* party = reinterpret_cast<CParty*>(GetParty());
    if (party != 0)
    {
        for (int i = 0; i <= 3; ++i)
        {
            CUser* u = party->get_user(i);
            if (u != 0 && this != u)   // P222 两形互试：4.1 产 `cmp %eax,0x8(%ebp)` mem-dst
                out.push_back(u);
        }
    }
    return party;
}
