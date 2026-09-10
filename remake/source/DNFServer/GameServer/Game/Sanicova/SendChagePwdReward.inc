void CPad::SendChagePwdReward(CUser* user)
{
    if (getAdvideChangePwd())
    {
        setAdvideChangePwd(false);
        if (user->getCurCharacNo() == 0)
            setChangedPwd(true);
        else
            SendChagePwdRewardMail(user);
    }
