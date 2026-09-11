// ----------------------------------------------------------------------------
// stMonsterCardBindInfo — C1/C2 W 0x0896dc80 range [...dd3d]
//   构造 3×vector<stMonsterCardBindResult> @+0x10（编译器成员默认构造等价），
//   尾调 clear()。
// clear W 0x0896dc38 range [...dc80]：i=0..3 { cnt[i]=0；vec[i].clear() }
// D1/D2 W 0x0896e218 range [...e250]：逆序析构 3 vector（编译器成员析构等价）
// ----------------------------------------------------------------------------
STExpertJobScript::stMonsterCardBindInfo::stMonsterCardBindInfo()
{
    clear();
}