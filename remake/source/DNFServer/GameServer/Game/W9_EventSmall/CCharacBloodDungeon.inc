// ============================ CCharacBloodDungeon ============================
CCharacBloodDungeon::CCharacBloodDungeon()
{
    // ORIG 0x832af6e：仅调基类 ctor 0x80c8cba + 置 vptr；帧内无字段清零
    // （ORIG 汇编无 memset/逐字段赋值），故与 ORIG 一致不做显式初始化。
}