// [b28c-lane17] ORIG ctor 0x08a5ba8c：两枚 pair 成员默认构造（+0x10/+0x18）
// 由编译器在初始化列表自动发射，ctor 体仅调 clear()。
MissionScript::MissionScript()
{
    clear();
}