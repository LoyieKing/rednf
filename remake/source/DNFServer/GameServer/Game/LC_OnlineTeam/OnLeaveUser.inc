// ORIG 0x08588ae8 T
// _ZN18online_preliminary25COnlinePreliminaryTeamMgr11OnLeaveUserEP5CUser
//（报告 OnLeaveUser.md）。
// [b28d-54 N] ORIG 0x08588ae8：槽整体平移 4（N82/N95）+ 迭代器/临时槽（N86）；
//   调用序列全等。
void online_preliminary::COnlinePreliminaryTeamMgr::OnLeaveUser(CUser* user)
{
    int teamId = user->GetCurCharacOnlinePreliminaryTeamId();  // 0x08645de2
    std::map<int, COnlinePreliminaryTeam*>::iterator it = m_teamMap.find(teamId);
    // [L6 round3 · p167 实测] ORIG 0x08588b40-0x08588b47 用
    // `_Rb_tree_iterator::ne`（!=）+ `test %al,%al; je <尾>` ⇒ **正极性包裹**
    // `if (it != end) { 体 }`（旧 `if (it == end) return;` 产 eq + jne 反向）。
    // 且 ORIG 逐点重算 `it->second`（`ptEv`×4：GetCurrentUserCount/saveData/Free/
    // DecreaseCurrentUserCount 各一次；我方 1 次 + MISSING 3 次）⇒ 删 `team` 具名缓存
    // （P183/P167，且其 -0xc 槽与 ORIG 的 end 临时槽 -0xc 冲突）。
    if (it != m_teamMap.end())
    {
        user->SetCurCharacOnlinePreliminaryTeam(0);            // 0x08588dba(team=0)
        if (it->second->GetCurrentUserCount() == 1)            // 0x08588d7e
        {
            it->second->saveData();                            // 0x08588d60
            m_factory->Free(it->second);                       // 0x08588dec（this+0）
            m_teamMap.erase(it);                               // 0x08589836
        }
        else
        {
            it->second->DecreaseCurrentUserCount();            // 0x08588da8
        }
    }
}