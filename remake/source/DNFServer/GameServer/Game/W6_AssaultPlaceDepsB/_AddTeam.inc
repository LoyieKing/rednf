void CAssaultPlace::_AddTeam(CUser* user, ENUM_ASSAULT_TEAM team)
{
    CParty* party = 0;
    if (user->CheckInParty())
        party = static_cast<CParty*>(user->GetParty());
    // b28 BL6: P59 双向布局——ORIG 0x82e70f2 `cmpl $0; je <else体后置>` 要求
    // 正极性 if(party!=0){for…}else{_AddUser}，体尾前向 jmp 共享出口。
    if (party != 0) {
        for (int i = 0; i <= 3; ++i) {           // cmpl $0x3 … setle
            CUser* member = party->get_user(i);
            if (member != 0)
                _AddUser(member, team);
        }
    } else {
        _AddUser(user, team);
    }
