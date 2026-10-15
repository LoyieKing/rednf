// ---- 插入条件 / 插入 / 删除 / 移动 ----
bool CAccountCargo::CheckInsertCondition(Inven_Item& item)
{
    // [b28d-solo-CAccountCargo 判词·DIFF（N 封顶）] ORIG 0x08289a4a：172 vs 175。
    // 已修真 bug：①0x08289bc6 虚调用 vptr+0x10 = _ZTV5CItem slot 4 = IsAvatarItem
    // （原误写 IsCreatureItem slot 5，P282）；②case 3 为 `m_field0 == 0 → return 0`
    // （ORIG `xor $1; test; je 继续`；原 `!= 1` 误拒 m_field0>=2 的物品）。
    // 已复现范式：isGM/avatar 嵌套否定早返（P59）、期限段1 条件极性
    // `usable==0 && expire!=0`（复现 `jne 段2` / `je false 块`）、GetLock 具名 uchar
    // 缓存（P0/P48）、find_item 实参先物化 %ebx（P24/P52）、expireSec 具名局部。
    // 残差封顶（不可复现）：①ORIG 6 项 switch 跳表（`cmp $5; ja; mov <A>(,%eax,4);
    //   jmp *%eax`）vs 4.4 比较链；补 case 0/4/5 撑宽值域反生 bitmask（P163 反例，
    //   实测已回退）；②虚调用 eax↔edx 寄存器对调（N7，4.4.7 间接 call 目标恒 %eax）；
    //   ③段1 else 的 return 1 未与尾部共享出口（4.4 独立发射，+2 条）。
    // p167 CLEAR（14/14，语义闭合）。
    // [b28d-46 P24/P52] ORIG 先把 item.m_addInfo 物化 %ebx 再 call G_CDataManager，
    // 链式形复现（具名 dm 局部使 receiver 先调用、实参后装载）。
    CItem* ci = G_CDataManager()->find_item(item.m_addInfo);
    if (ci == 0)
        return 0;
    // [b28d-46 P59/P168a] ORIG `call isPackagable; xor $1; test %al; je BODY;
    // mov $0;jmp END; BODY:` ⇒ 早期返回 guard 形（非 if 包裹 + 尾部 return 0）。
    if (!ci->isPackagable())
        return 0;
    {
        // [b28d-46 P0/P48] ORIG 把 GetLock() 存 bool 槽 -0x15（cmpb 判定）后复用；
        // 内联两次调用形在 4.4 多一次 call（无 CSE）⇒ 具名 uchar 局部。
        unsigned char lock = item.m_amp.GetLock();
        if (lock != 0) {
            if (((item_lock::CItemLock*)m_pUser->GetCharacExpandDataR(
                    (ENUM_CHARAC_EXPAND_TYPE)2))->CheckItemLock(lock))
                return 0;
        }
        if (item.m_itemType == 4 || item.m_itemType == 5 || item.m_itemType == 6 ||
            item.m_itemType == 7 || item.m_itemType == 8)
            return 0;
        // ORIG 0x8289a4a +0xe7/+0xed：m_addInfo 落在 [0x1964, 0x1b57](6999) 区间内才
        // 直接返回 0；区间之外继续后续检查（GETATTACH 等）。
        if ((unsigned int)item.m_addInfo >= 0x1964 &&
            (unsigned int)item.m_addInfo <= 0x1b57)
            return 0;
        // [b28d-solo-CAccountCargo] ORIG 0x08289b48：switch 值有具名槽 -0x14
        // （P0/P28），跳表覆盖 0..5（P163 补未列 case 0/4/5 撑宽 range）；
        // case 3 判定为 `m_field0 == 0 → return 0`（ORIG `xor $1; test; je 继续`，
        // 原 `!= 1` 会误拒 m_field0>=2 的物品=真语义 bug）。
        // [L8 2026-09-10 判据⑨ 旧判词重核] ORIG 0x08289b56 段为**跳转表**
        // （`mov -0x14(%ebp),%eax; cmp $0x5,%eax; ja +0x13e; mov <A>(,%eax,4),%eax;
        // jmp *%eax`，6 项 0..5）且用**无符号** `ja`。旧判词「补 case 0/4/5 反生
        // bitmask」是在 `int` 表达式下测的（有符号 lowering）；改 **unsigned** 并补
        // [L8 实测] 补 case 0/4/5 反使 4.4 走 **bitmask**（`mov $0x1,%edx; shl %cl,%ebx`
        // 段，189 条 > 原 177）⇒ 已回退补 case，仅保留 **unsigned** 表达式。
        // [2026-09-11 负结论·ORIG 跳表已 dump] 读 ORIG 0x08c14820 处 6 项跳表：
        // [0]=0x08289b88(↓IsTradeRestriction 段，即 default/继续)、[1]=[2]=0x08289b67
        // (return 0)、[3]=0x08289b71(case3 体)、[4]=[5]=0x08289b88 ⇒ ORIG 源码实有
        // 6 个 case（0/4/5 空体）。两形实测均**更差**：
        //   ① 6 个独立 case（0/4/5 `break;`）⇒ 4.4 发 `cmp $0x2; ja` + 双层比较（177 条）；
        //   ② case 1/2 各自 `return 0;` ⇒ 发重复 `cmp $0x1; je` + 比较链（181 条）。
        // ⇒ 4.4 的 `expand_case` 对本值域不选跳表（旧 bitmask 结论同为负），**封顶**。
        unsigned int attachType = (unsigned int)ci->GetAttachType();
        switch (attachType) {
        case 1:
        case 2:
            return 0;
        case 3:
            if (item.m_seal == 0)
                return 0;
            break;
        }
        if (item.m_upgradeSep.IsTradeRestriction())
            return 0;
        // [b28d-46 判词·真 bug/P282] ORIG 0x08289bc6 虚调用 vptr+0x10 = CItem vtable
        // slot 4 = IsAvatarItem（非 slot 5 IsCreatureItem）；按 _ZTV5CItem 0x08c96da0
        // 逐槽核对（vptr=_ZTV+8，slot12=IsImpossibleUpgrade 与头声明序吻合）。
        // [b28d-solo-CAccountCargo] ORIG 0x08289ba9/0x08289bc1：GM/avatar 为两个
        // 独立早返（各自 `xor $1; test; je <return 1 岛>`，P63/P133），非合并 &&。
        if (!m_pUser->isGMUser()) {
            if (!ci->IsAvatarItem()) {
                // [2026-09-11 语义修正] ORIG 0x08289bde..0x08289c0a 的判据是
                // `!(usable == 0 && expire == 0)`：objdump 形为
                // `call getUsablePeriod; test; jne <TRUE>; call getExpirationDate;
                //  test; je <FALSE>; TRUE: mov $1; jmp <conv>; FALSE: mov $0;
                //  conv: test %al,%al; je <return 1 岛>` —— 即“两者都为 0 才跳过”，
                // 等价 `usable != 0 || expire != 0`（与 Ghidra bVar2 一致）。
                // 原实现写成 `usable == 0 && expire != 0`（`jne`+`je` 均落 FALSE）
                // ⇒ 有使用期限(usable!=0) 的物品被误放行、跳过到期时间检查 = 真语义 bug。
                if (!(ci->getUsablePeriod() == 0 &&
                      ci->getExpirationDate() == 0)) {
                    // ORIG 0x08289c0c..0x08289c36：内层重复同一对调用，极性为
                    // `a == 0 && b == 0`（两跳均 `jne <FALSE>`）→ 命中即 return 0
                    // （外层为真时不可达，ORIG 原样保留该双调用形）。
                    if (ci->getUsablePeriod() == 0 &&
                        ci->getExpirationDate() == 0) {
                        return 0;
                    }
                    // [b28d-46 P0] ORIG 把 imul/add 结果先落 -0x10 槽再 call
                    // getCurSec（具名 expireSec）；endurance 内联 item.m_endurance。
                    int expireSec =
                        (int)((unsigned int)item.m_endurance * 0x15180 + 0x44a54a80);
                    int curSec = GlobalData::s_systemTime_.getCurSec();
                    if (expireSec <= curSec)
                        return 0;
                }
            }
        }
        return 1;
    }
}