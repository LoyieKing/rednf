void CInvalidGold::execute()
{
    CUser* user = GetUser();
    const unsigned int oldMoney = user->getCurCharacMoney();
    // ORIG .data 0x093abb40 初值 ffffd8f0（-10000）；每次执行再减 10000。
    static int invalidMoney = (int)0xffffd8f0u;
    invalidMoney -= 10000;
    user->getCurCharacInvenW()->set_money(invalidMoney);
    user->SendUpdateItemList(CUser::eSendTarget_1, ENUM_ITEMSPACE_0, 0);
    // b28d-331：ORIG 末段 `mov -0xc(%ebp),%ebx` 在 receiver call 前装载
    // （4.1 实参先求值）⇒ 经裸 register 中转局部（P231/P248 合规，非 asm 绑定）
    // 令 4.4 提前物化入 %ebx 并复用 oldMoney 槽（探针 /tmp/b28d_gmd/p2.cpp f4 同形）。
    register unsigned int restoreMoney = oldMoney;
    user->getCurCharacInvenW()->set_money(restoreMoney);
