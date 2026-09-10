SecretShopScript::SecretShopScript()
{
    clear();   // ORIG C1 0x83720f4：构造 5 容器成员后调 clear()（写 +0x80/+0x84）
