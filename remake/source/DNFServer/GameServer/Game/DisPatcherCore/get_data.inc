int DisPatcher_BuyItem::get_data(CUser* user, MSG_BUY_ITEM& msg, BuyItemParam& param)

{
    // [G5-MiscC] P0/P52：ORIG 0x081be660 `mov 0x10(%ebp),%eax; mov 0xd(%eax),%eax;
    // mov %eax,%ebx` 先把 msg.m_itemIdx 存入 callee-saved %ebx，再
    // 0x081be668 `call G_CDataManager; mov %ebx,0x4(%esp); mov %eax,(%esp);
    // call find_item`（无 dataMgr 具名槽；我方 fresh 多 -0x1c 槽）⇒ 单语句链式。
    param.m_a8 = G_CDataManager()->find_item(msg.m_itemIdx);
    if (param.m_a8 == 0)
    {
        return 0x11;
    }
    // GetNeedMaterial() 返回 +0x24 处 pair<int,int>*（item idx, 数量），ORIG 读两 int
    // [G5-MiscC] ORIG 0x081be6a5 只调用一次、结果直接在 %eax 上被取走两 int：
    // `mov 0x4(%eax),%edx; mov (%eax),%eax`（单一 disp，无 add）⇒ 指针视图按
    // pair 访问（原 `const int*` + `[1]` 在 4.4 生成 `add $0x4,%eax` 两步寻址）。
    std::pair<int, int>* needMaterial =
        reinterpret_cast<std::pair<int, int>*>(param.m_a8->GetNeedMaterial());
    param.m_pair.first = needMaterial->first;
    param.m_pair.second = needMaterial->second;
    if (param.m_pair.first != -1)
    {
        // [G5-MiscC] P0/P183：ORIG 0x081be6d3 `lea 0x41(%eax),%esi`（&m_resultItem）/
        // 0x081be6d9 `mov 0x98(%eax),%ebx`（pair.first）/ 0x081be6e5
        // `mov 0xc(%ebp),%eax; mov %eax,(%esp); call getCurCharacInvenR`（对象最后求值）
        // —— 无 invenR 具名槽（我方 fresh 多 -0x10）⇒ 内联链式。
        param.m_a4 = user->getCurCharacInvenR()->GetInvenData(
            param.m_pair.first, param.m_resultItem);
        // [G5-MiscC] P0：ORIG 0x081be706 `mov 0x98(%eax),%ebx; call G_CDataManager;
        // mov %ebx,0x4(%esp); ... call find_item`（无 dataMgr2 具名槽，我方 fresh 多 -0xc）
        // ⇒ 内联链式（itemIdx 亦在 %ebx 保活）。
        param.m_ac = G_CDataManager()->find_item(param.m_pair.first);
        if (param.m_ac == 0 || param.m_ac->is_stackable() != true)
        {
            return 1;
        }
    }
    param.m_80 = param.m_a8->get_price();             // ORIG 0x0822c84a W
    param.m_84 = param.m_a8->GetCashPrice();          // ORIG 0x0822c856 W
    param.m_88 = param.m_a8->GetMedalPrice();
    param.m_8c = param.m_a8->GetFinishPointPrice();
    param.m_90 = param.m_a8->GetGuildPowerWarPoint();   // ORIG vtable slot +0x28
    param.m_94 = param.m_a8->GetUserPowerWarPoint();    // ORIG vtable slot +0x2c
    if (param.m_a8->is_stackable() != true)
    {   // 非堆叠：msg+0x11 须等于当日种子（防复制）
        if (msg.m_buyCount != user->GetSeedFromDate())
        {
            return 1;
        }
    }
    else
    {
        if (msg.m_buyCount == 0)
        {
            return 1;
        }
        if ((unsigned int)msg.m_buyCount > 1)
        {
            // [G5-MiscC] P0 反向：ORIG 0x081be87c `mov %eax,-0xc(%ebp)` 有槽往返
            // （0x081be87f `mov -0xc(%ebp),%edx; mov 0x10(%ebp),%eax;
            // mov 0x11(%eax),%eax; cmp %eax,%edx; jae`）⇒ 原码确有具名 unsigned int
            // 局部（我方原内联调用无槽）。
            unsigned int maxBuyableCount = get_nMaxBuyableCount(user, param);
            if (maxBuyableCount < (unsigned int)msg.m_buyCount)
            {
                return 10;
            }
            param.m_80 *= msg.m_buyCount;
            param.m_84 *= msg.m_buyCount;
            param.m_88 *= msg.m_buyCount;
            param.m_8c *= msg.m_buyCount;
            param.m_90 *= msg.m_buyCount;
            param.m_94 *= msg.m_buyCount;
            param.m_pair.second = msg.m_buyCount * param.m_pair.second;
        }
    }
    // ---- 余额校验：任一货币不足 → 10 ----
    // [DPC_CheckErr] 轴 B 符号性（语义轴，按 ORIG cc 双向核）：ORIG 0x081be990-0x081be99b
    // `mov 0x80(%edx),%edx; cmp %edx,%eax; jl 0x081bea41` —— **有符号** jl ⇒ 原码比较
    // 两侧为 int（get_money() 返回 int，Ghidra 亦转写 `iVar10 < *(int*)(param+0x80)`）。
    // `param.m_80` 在本 .h 声明为 unsigned int（+0x80 P211 需要），故必须显式
    // `(int)param.m_80` 才能让 4.4 产 `jl`；原形 `(unsigned int)get_money() <
    // (unsigned int)param.m_80` 产 `jb`（无符号）⇒ 负余额（money<0）时行为分叉。
    // 同链其余货币（GetCera/GetWinPoint/GetFinishPoint/GetGuildPowerWarPoint）
    // ORIG 均为 `jb`（0x081be9b7/0x081be9d3/0x081be9ed/0x081bea26）⇒ 那四处保留
    // unsigned 形；GetUserPowerWarPoint 为 16 位有符号（0x081bea3c `cmp %dx,%ax; jge`）
    // ⇒ 保留 (short) 形。
    bool insufficient =
        (user->getCurCharacInvenR()->get_money() < (int)param.m_80) ||
        ((unsigned int)user->GetCera() < (unsigned int)param.m_84) ||
        ((unsigned int)user->GetWinPoint() < (unsigned int)param.m_88) ||
        ((unsigned int)user->GetFinishPoint() < (unsigned int)param.m_8c) ||
        ((param.m_pair.first != -1 &&
          param.m_resultItem.m_addInfo2 < param.m_pair.second)) ||
        ((unsigned int)user->GetGuildPowerWarPoint() < (unsigned int)param.m_90) ||
        ((short)user->GetUserPowerWarPoint() < (short)param.m_94);
    if (insufficient)
    {
        return 10;
    }
    // 公会点数商品仅会长可购（vtable slot +0x28 复读）
    if (param.m_a8->GetGuildPowerWarPoint() != 0 && !user->IsGuildMaster())
    {
        return 0x24;
    }
    return 0;
}
