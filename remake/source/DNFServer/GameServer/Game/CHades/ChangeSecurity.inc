// ORIG 0x084b95a8（ChangeSecurity.md）：仅转调 needLogin()，参数未参与逻辑。
void CHades::ChangeSecurity(ENUM_HADES_SECURITY type, unsigned int value)
{
    (void)type;
    (void)value;
    needLogin();
}