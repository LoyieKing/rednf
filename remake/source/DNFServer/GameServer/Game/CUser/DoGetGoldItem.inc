bool CUser::DoGetGoldItem(int itemIdx, std::vector<std::pair<int, int> >& out,
                          bool bReset, ENUM_ERROR& err)
{
    int amount = 0;
    if (itemIdx <= 0x28d291)
        amount = (itemIdx - 0x28d287) * 1000000;
    else
        amount = itemIdx * 10000000 - 0x549a800;

    if (bReset)
    {
        // ORIG 0x0869290a/-0x048 预清 `movl $0x7fffffff,-0xc` 后覆盖 ⇒ 两句式（P198①）
        int limit = 0x7fffffff;
        limit = G_CDataManager()->GetMoneyLimitPerLevel(
            get_charac_level(), get_acc_name());
        // ORIG 0x0869294e..0x098：`mov -0xc,%edx; mov %edx,%ecx; sub %eax,%ecx;
        // mov %ecx,%eax; cmp -0x10(%ebp),%eax; setl %al` ⇒ 有符号 `limit - money < amount`
        // （轴 B：ORIG setl ⇒ 去掉 unsigned cast，勿写 money+amount>limit 的无符号形）
        if (limit - getCurCharacMoney() < amount)
        {
            err = (ENUM_ERROR)0xa;
            return false;
        }
    }
    else
    {
        // ORIG 0x08692978..0x086929a5：getCurCharacInvenW 结果留 %eax 直跨到 gain_money
        // （无具名指针槽）⇒ P183 不缓存 receiver 指针
        getCurCharacInvenW()->gain_money(amount, (eMoneyAddReason)0xe, true, 0);
        // b28d-solo-CUser p167：ORIG 0x086928c2 实例化
        // `make_pair<ENUM_ITEMSPACE, Inven_Item::INVEN_SLOT>`（我方两 int）。
        out.push_back(std::make_pair(ENUM_ITEMSPACE_0, (Inven_Item::INVEN_SLOT)0));
    }
    return true;
