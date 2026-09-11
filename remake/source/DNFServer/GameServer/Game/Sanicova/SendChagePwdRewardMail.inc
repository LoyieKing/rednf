// ORIG W 0x084ebdb2：写 +0x3c 字节。
// P210：本 setter 与 setChangedPwd 定义位于 SendChagePwdRewardMail 之后，
// 使 4.4 对该 callee 采用保守帧（ORIG sub $0x18）。
// ORIG T 0x0859981e：置密改与建议标记为 0（发奖邮件已由上层处理）。
// 定义前置到 setter 之前（P210 callee 定义顺序定律）。
void CPad::SendChagePwdRewardMail(CUser* user)
{
    setChangedPwd(false);
    setAdvideChangePwd(false);
}