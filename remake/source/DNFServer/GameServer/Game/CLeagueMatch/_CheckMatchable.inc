// [b28d-solo-CLeagueMatch] AE（TU 口径）：69=69 全等。修法=P225 `while(cond){return X;}`
// 复现 ORIG 尾 `cmp %eax,%edx; setne %al; test %al,%al; je` 的 bool 四段物化
// （三目/if 直跳形恒被 4.4 折成 `jne`，探针 /tmp/b28d_cmatch 两形实测：三目
// ndiff 8、bool 局部占槽反劣 13、while+break 归零）；槽序按 P0 对齐
// （emptySeats/X/i/user，X=logArg0 复现 -0x14 具名局部，删 school 局部改两次
// GetSchoolNo P48）。
int CLeagueMatch::_CheckMatchable(PvP_Room* room) const   // ORIG T 0x0855da56
{
    int emptySeats = 0;
    // [b28d-solo-CLeagueMatch] P0：ORIG -0x14 具名局部（恒 0）作 logFormat 首个 %d
    // 实参（0x0855dad6 `mov -0x14(%ebp),%eax; mov %eax,0x14(%esp)`）；删除 school
    // 局部改两次 GetSchoolNo（P48 O0 无 CSE），槽序 emptySeats/X/i/user 与 ORIG 全等。
    int logArg0 = 0;
    for (int i = 0; i <= 7; ++i) {
        if (room->get_team(i) == 3) {
            ++emptySeats;
            continue;
        }
        CUser* user = reinterpret_cast<CUser*>(room->get_user_seat(i));
        if (user == 0)
            continue;
        if (user->GetSchoolNo() == 0) {
            LogManager::logFormat(
                1, "MatchingSystem.cpp",
                __PRETTY_FUNCTION__, 599,
                "(%d) found a different school no(%d)", logArg0, user->GetSchoolNo());
            return 0x16;
        }
    }
    while (m_teamA.size() - emptySeats != (int)nPlayerCountPerOneTeam_) {
        return 0x16;
    }
    return 0;
}