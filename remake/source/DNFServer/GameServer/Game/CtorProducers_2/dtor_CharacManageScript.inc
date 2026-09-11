CharacManageScript::~CharacManageScript()
{
    // ORIG 0x088e07f6：逆序析构 +0x7c vector → 4×map → +0xc vector → +0x00
    // vector<ManagePointBonus>（成员自动，顺序一致）。
}