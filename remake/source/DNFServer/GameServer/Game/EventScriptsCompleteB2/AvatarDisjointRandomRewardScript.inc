// ORIG W 0x08985be2：基类 → map<char,uint>+0x8 → mail+0x20 → mail.reset。
AvatarDisjointRandomRewardScript::AvatarDisjointRandomRewardScript()
{
    m_mail.reset();
}