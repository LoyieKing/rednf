// [S414_DB_Login] verdict=AE ndiff=0（82=82）：P286 单 case switch 翻正两处
// `== 常量` 比较——ORIG 0x08414772 段 `mov -0x14(%ebp),%eax; cmp $K,%eax; jne`
// 两段寄存器形；if 形 4.4 恒折 `cmpl $K,-0x14(%ebp)`（旧 N-P209L 判词作废）。
// 前史（S216）：P235 数组视角（m_characView）修 memmove src/dst 与槽比较地址链，
// int==unsigned 异型比较修两段装载（P211 档③），22→6。
int DB_Login::GetGameDBInfo(SIG_LOGIN_DATA* sig)
{
    // [S414/P286] ORIG 两次比较均两段寄存器形（`mov -0x14(%ebp),%eax; cmp $imm,%eax`）；
    // 单 case switch 形令 4.4 同样先载槽再寄存器比较（if 形恒折 `cmpl $imm,mem`）。
    int viewRet = _getGameDBCharacView(sig);
    // [S414/P286] 单 case switch：复现 ORIG `mov -0x14(%ebp),%eax;
    // cmp $0xffffffff,%eax; jne` / `cmp $1,%eax; jne` 两段寄存器形。
    switch (viewRet)
    {
    case 0xffffffff:
        return 1;
    default:
        break;
    }
    switch (viewRet)
    {
    case 1:
        return 0;
    default:
        break;
    }
    // [b28d-solo-DB_Login] P0：ORIG 无 world 具名局部（G_GameWorld 返回值直传
    // GetChannelType）；P222 正极性包裹复现 `sete; test; je <整块>`。
    if (G_GameWorld()->GetChannelType() == 7)
    {
        // P28：ORIG 槽 i=-0x10（深）/count=-0xc ⇒ 先声明 i 后 count。
        int i = 0;
        int count = sig->m_characCount;          // +0xdc 槽计数
        for (; i < count; ++i)
        {
            if (sig->m_characView.m_slots[i].m_characNo ==
                sig->m_tournamentAccountFlag)           // server_id 匹配（int==unsigned 异型，P211 档③）
            {
                memmove(&sig->m_characView.m_slots[0],
                        &sig->m_characView.m_slots[i], 0x94);
                if (sig->m_broadcastCharacName[0] != 0)       // charac_name 非空才覆写（SigTypes_D 隐式区）
                {
                    strncpy(sig->m_characData + 4, sig->m_broadcastCharacName, 0x1e);
                }
                sig->m_characCount = 1;
                break;
            }
        }
        if (i == count)
        {
            return 1;
        }
    }
    return 0;
}