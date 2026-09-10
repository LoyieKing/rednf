bool CParty::is_limit_minimum_age(int age, int& param)
{
    for (int i = 0; i <= 3; i++)
    {
        // [判据②直分支形+拆&&] ORIG O@26-31：`call;xor$1;test;je` +
        // `mov$0;jmp` 直返，无 && 物化岛——两条件拆嵌套。
        if (_checkValidUser(i))
        {
            if (m_member[i].m_pUser->CheckLimitMiniumAge(age, param) == 0)
            {
                return false;
            }
        }
    }
    return true;
