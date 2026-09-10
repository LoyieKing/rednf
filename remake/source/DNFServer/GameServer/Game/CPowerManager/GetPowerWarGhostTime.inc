int CPowerManager::GetPowerWarGhostTime(CUser* user, float f)
{
    // ORIG 0x08480e92：(int)(ghostTime * f)，fistpl 就近取整。
    (void)user;
    // [b28c-L9] ORIG 帧 sub $0x28（结果槽 -0xc、转换源 -0x1c、cw -0x1e/-0x20），
    // 4.4 仅 $0x18 ⇒ P190 double framePad 撑帧；残余 2 = N82（4.1 不复用 -0xc/-0x1c）。
    // [b28d-solo] P0：ORIG 两个槽往返（源 -0x1c / 结果 -0xc）⇒ 原码有两个具名
    // 局部（4.4 对匿名转换临时会复用源槽）。
    int ghostTime = G_CDataManager()->m_powerParameter.m_ghostTime;
    int n = (int)((float)ghostTime * f);
    double framePad;
    (void)&framePad;
    return n;
