// ORIG execute 0x084b3c60：m_str0=="mobile" 时改 garena auth data +0x2c 字节并打日志；
// =="character" 分支仅做比较（结果丢弃，空体）。
// b28d-solo-GmCmds_E：P227 全表达式临时链——ORIG 在 cMyTrace ctor 之前把
// yes/no 三元物化进 %ebx 并跨 ctor 保活（`mov $yes,%ebx` 在 C1 前），具名
// yesNo 局部会被 4.4 槽化；改临时链 + 三元内联复现。
void CMobileRestrictCommand::execute()
{
    CUser* user = GetUser();
    if (m_str0 == "mobile")
    {
        // ORIG 0x084b3cf0 `mov %dl,0x2c(%eax)`（完整类型成员直访问，无中间槽）。
        reinterpret_cast<Taiwan::GarenaAuthData*>(user->getGarenaAuthData())
            ->m_mobileAuth = static_cast<unsigned char>(m_restrictSwitch);
        cMyTrace(__PRETTY_FUNCTION__, 0x183d, 0)(
            "[GMCommand] Set mobile restricted. mobile auth:%s",
            m_restrictSwitch == 1 ? "yes" : "no");
    }
    else if (m_str0 == "character")
    {
        // ORIG 仅比较、无副作用
    }
}