// ORIG 0x081be658 T get_data（报告 get_data.md ## 反编译 C 全文映射）：
// 解析商城物品 → 材料槽 → 六种单价（>1 件时乘数量并校验上限）→ 持有量校验。
// [b28d-29] N82/N18a：帧 0x20 vs 0x34 + 返回指针缓存槽（-0x1c）与两段中转差；语义集合全等  (ORIG 0x081be658 insn 310->328)
// 返回 0=成功；0x11=物品不存在；1=参数非法；10=余额不足；0x24=公会会长限制。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 265（ORIG 310 / ours 327），残差=N7/N82 帧与 callee-saved 角色/setcc-fold bool 物化岛/P168a④ bool 槽化/P211/N18a；沿用上方 b28d-29 归因，封顶。
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
    // [DpcMisc-P2 2026-09-11] ORIG 0x081be6a5-0x081be6bc 只调用一次、无任何具名指针槽：
    //   `call GetNeedMaterial; mov 0x14(%ebp),%ecx; mov 0x4(%eax),%edx; mov (%eax),%eax;
    //    mov %eax,0x98(%ecx); mov %edx,0x9c(%ecx)`
    // （dest=param 缓存 %ecx + 两读交错 = 4.1 的 8 字节块拷贝形）⇒ 原码是**结构整体赋值**
    // 而非「具名指针 + 逐字段访问」；旧形 `std::pair<int,int>* needMaterial = …` 在 4.4
    // 生成 -0x14 槽 + 3 次槽回读（共 6 条额外指令，本件 gap=+10 的主要来源）。
    param.m_pair =
        *reinterpret_cast<std::pair<int, int>*>(param.m_a8->GetNeedMaterial());
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
    // [R20a01 §26.3 多余物化] ORIG 0x081be990..0x081bea4d 该布尔链**无栈槽**
    // （0x081bea4d `test %al,%al` 直测寄存器；失败项各 `je 0x081bea41 mov $1`），
    // 我方具名 `bool insufficient` 多出 `mov %al,-0xd(%ebp); cmpb $0x0,-0xd(%ebp)` 两条
    // ⇒ 改内联 if（短路链与失败出口不变）。
    if (
        (user->getCurCharacInvenR()->get_money() < (int)param.m_80) ||
        ((unsigned int)user->GetCera() < (unsigned int)param.m_84) ||
        ((unsigned int)user->GetWinPoint() < (unsigned int)param.m_88) ||
        ((unsigned int)user->GetFinishPoint() < (unsigned int)param.m_8c) ||
        ((param.m_pair.first != -1 &&
          param.m_resultItem.m_addInfo2 < param.m_pair.second)) ||
        ((unsigned int)user->GetGuildPowerWarPoint() < (unsigned int)param.m_90) ||
        ((short)user->GetUserPowerWarPoint() < (short)param.m_94))
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
