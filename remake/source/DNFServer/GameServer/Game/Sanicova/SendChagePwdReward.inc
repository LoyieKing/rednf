// ORIG T 0x085997b8：建议改密时复位建议并触发发奖（无角色则仅置密改标记）。
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
}