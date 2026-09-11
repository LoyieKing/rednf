// ---------------------------------------------------------------------------
// ORIG T 0x82e8098 (size 0x209)：_SeparateTeam(CUser*, CUser*)
// 双队长各自队伍列出并布阵。依赖 _AddUser/_MoveUser/_MoveAssaultPlace/
// _SendUserUdpIpPort（B 组交付）。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_AssaultPlaceDepsA] AE：users[2] 头部零初始化（ORIG 0x082e80a1 双
//   movl $0 证据）+ party 槽复用 + at 内联实参 + 去三目 ⇒ 167=167 对齐；
//   p167 CLEAR。ORIG 0x082e8098
void CAssaultPlace::_SeparateTeam(CUser* leader1, CUser* leader2)
{
    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082e80a1/0x082e80a8 函数头两条
    //   `movl $0x0,-0x2c/-0x28` ⇒ users[2] 在函数头声明并零初始化（P0 槽证据）；
    //   末尾 +0x16b/+0x171 才写 leader1/leader2。
    CUser* users[2] = { 0, 0 };
    std::vector<CUser*> lists[2];
    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082e8123/0x082e8155 两次写同一槽
    //   -0x24（GetUserListByParty 结果复用）⇒ 单个具名局部两次赋值（P0 槽复用）。
    CParty* party = leader1->GetUserListByParty(lists[0]);
    _BackupPartyInfo(party, (ENUM_ASSAULT_TEAM)0);
    party = leader2->GetUserListByParty(lists[1]);
    _BackupPartyInfo(party, (ENUM_ASSAULT_TEAM)1);

    for (unsigned int u = 0; u < 8; ++u) {
        for (int t = 0; t < 2; ++t) {
            if (lists[t].size() > u) {
                // ORIG 0x082e81cc at 结果 (%eax) 直传 _AddUser，无 member 具名槽。
                _AddUser(lists[t].at(u), (ENUM_ASSAULT_TEAM)t);
            }
        }
    }

    users[0] = leader1;
    users[1] = leader2;

    // ORIG 0x082e821a `test %eax,%eax; setg %dl; mov %dl,0x128(%eax)` ⇒ 直接
    // 布尔存字节（三目 ?1:0 多归一化段）。
    m_chaosFlags[0] =
        ((CUserCharacInfo*)leader1)->GetCurCharacChaosPoint() > 0;
    m_chaosFlags[1] =
        ((CUserCharacInfo*)leader2)->GetCurCharacChaosPoint() > 0;

    _DispositionUser(users);
}