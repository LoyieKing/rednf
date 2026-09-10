void AvatarRechargeServer::CalcDurability(CUser* user, int count)
{
    for (int round = 0; round < count; ++round)
    {
        std::vector<int> rates;
        rates.reserve(10);
        for (int slot = 0; slot <= 9; ++slot)
        {
            // [b28d-solo-AvatarRechargeServer] ORIG 0x081904f5 `xor $1,%eax; test %al,%al`
            // （je 落循环增量处被 4.1 jump-thread）⇒ 原码是 `if (!Detail(...)) continue;`。
            if (!CalcDurabilityDetail(user, slot, rates))
                continue;
        }
        if (!rates.empty())
        {
            // [b28d-solo-AvatarRechargeServer] P13 循环 end 缓存：ORIG 0x08190562
            // `call size; mov %eax,-0xc` 每轮不重算，条件 `cmp -0xc,%eax; setb`。
            for (std::vector<int>::size_type i = 0, n = rates.size(); i < n; ++i)
                UpdateAvatarEndurance((CUser::eSendTarget)2, user,
                                      (short)rates[i]);
        }
    }
