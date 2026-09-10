bool AvatarRechargeServer::CalcDurabilityDetail(CUser* user, int equipSlot,
                                                std::vector<int>& rates)
{
    // ORIG 调用序：getCurCharacInvenRefW(user) → GetInvenRef(inven, 0, equipSlot)。
    // ORIG 无 inven 具名槽（P0：结果 %eax 直传 GetInvenRef 首参；equipSlot 跨
    // 该 call 由 callee-saved 保活）⇒ 内联成员链，勿落局部（W6：旧形多 1 槽
    // ⇒ 全槽后移 + 帧 0x28→0x38）。
    Inven_Item* item = user->getCurCharacInvenRefW()->GetInvenRef(0, equipSlot);
    if (item->isEmpty() || item->m_fieldSplit.m_avatarEndur == 0)
        return false;

    // ORIG：getCurCharacInvenW(user) → GetAvatarItemMgrW(inven)。
    WongWork::CAvatarItemMgr* mgr =
        user->getCurCharacInvenW()->GetAvatarItemMgrW();

    // 减免率：GM 开关命中取 GM 值低 16 位，否则取徽章耐久表值低 16 位。
    // ORIG 有 `movw $0x0,-0xa` 预清（具名 short rate = 0）+ `xor $1` 反查
    // 开关（!m_bUseEnduranceReduceRate 走 GetEmblemReduceRate 分支）。
    short rate = 0;
    if (!m_bUseEnduranceReduceRate)
        rate = static_cast<short>(mgr->GetEmblemReduceRate(item->m_addInfo2));
    else
        rate = static_cast<short>(m_enduranceReduceRate);

    // ORIG：三守卫 `jle/je/setl-je` 三条 jcc 同落 +0x122 尾 `mov $1,%eax`，零岛。
    // [C05③] 独立 `if(cond) goto L;` 形在 4.4 下每条产 `jump+nop` 岛（+6）；
    // 合并 `||` 实测净零（物化抵消）。改正向嵌套：4.4 把 fail 路径 jump-thread 到
    // 共享尾出口，无岛（P223 定案①：正向嵌套恒被 thread 成 0 岛）。求值序同 ORIG。
    if (rate > 0)
    {
        if (item->m_fieldSplit.m_avatarEndur != 0)
        {
            if (get_rand_int(1000) < rate)
            {
                item->m_fieldSplit.m_avatarEndur -= 1;
                mgr->UpdateEndurance(item->m_addInfo2, item->m_fieldSplit.m_avatarEndur);
                rates.push_back(equipSlot);
            }
        }
    }
    return true;
