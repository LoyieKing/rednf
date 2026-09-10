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
