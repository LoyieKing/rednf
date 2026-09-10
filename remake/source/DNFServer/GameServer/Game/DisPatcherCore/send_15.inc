void DisPatcher_BuySkill::send(CUser* user, ParamBase& param)
{
    stOnBuySkillParam* local_18 = &static_cast<stOnBuySkillParam&>(param);
    PacketGuard local_24;
    local_24.put_header(1, 0x1f);
    if (local_18->m_350 < 0)
    {
        local_24.put_byte(0);
        // [b28d] P324：ORIG 叶块物理序 = 书写序 = 0x081bf4de(-1)/0x081bf4f3(-2)/
        // 0x081bf508(-3)/0x081bf51d(-4)/0x081bf532(-5)/0x081bf547(-6) ⇒ 降序书写。
        switch (local_18->m_350)
        {
        case -1:
            local_24.put_byte(0x15);
            break;
        case -2:
            local_24.put_byte(0x16);
            break;
        case -3:
            local_24.put_byte(0x12);
            break;
        case -4:
            local_24.put_byte(1);
            break;
        case -5:
            local_24.put_byte(0x5b);
            break;
        case -6:
            local_24.put_byte(0x5c);
            break;
        }
        local_24.put_byte(static_cast<int>(local_18->m_354));
    }
    else
    {
        local_24.put_byte(1);
        // [b28d] P0/P183/P329：ORIG 0x081bf58e/0x081bf5ab 无 cVar2/uVar3/pSVar4/iVar5
        // 具名局部——put_byte 实参直取 (char)GetCurCharacSkillTreeIndex()；uVar3 经
        // %ebx 跨 getCurCharacSkillR 保活（嵌套实参右→左求值），iVar5 直传 put_short。
        local_24.put_byte(static_cast<int>(static_cast<char>(user->GetCurCharacSkillTreeIndex())));
        local_24.put_short(
            reinterpret_cast<SkillSlot*>(const_cast<char*>(user->getCurCharacSkillR()))
                ->get_remain_sp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(
                    user->GetCurCharacSkillTreeIndex())));
        if (user->GetCurCharacSkillTreeIndex() == -1 ||
            user->GetCurCharacSkillTreeIndex() == 0)
        {
            local_24.put_short(
                reinterpret_cast<SkillSlot*>(const_cast<char*>(user->getCurCharacSkillR()))
                    ->get_remain_sfp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(2)));
        }
        else
        {
            local_24.put_short(
                reinterpret_cast<SkillSlot*>(const_cast<char*>(user->getCurCharacSkillR()))
                    ->get_remain_sfp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(3)));
        }
        local_24.put_byte(static_cast<int>(static_cast<char>(local_18->m_355)));
        for (int local_14 = 0; local_14 < static_cast<int>(static_cast<char>(local_18->m_355));
             local_14 = local_14 + 1)
        {
            // [b28d] P183：ORIG 0x081bf68b 起每处字段访问重发完整
            // `mov i; mov base; add; add; shl; movzbl 0x8(%eax,%ecx,1)` 下标链，
            // 无 `buySkillInfo& entry` 引用缓存（缓存会少 6 条/处并撑槽）。
            // [b28d] P214①/P272：ORIG fall-through 分支是 0xff 分支
            // （`cmp $0xc7; jbe <m_4 分支>` + `cmp $0xd0; ja <m_4 分支>`），
            // 故源形取正条件 `m_0>0xc7 && m_0<=0xd0` 的 if 包裹。
            if (static_cast<unsigned char>(local_18->m_infos[local_14].m_0) > 0xc7 &&
                static_cast<unsigned char>(local_18->m_infos[local_14].m_0) <= 0xd0)
            {
                local_24.put_byte(0xff);
                local_24.put_byte(static_cast<unsigned int>(static_cast<unsigned char>(
                    local_18->m_infos[local_14].m_0)));
                local_24.put_byte(local_18->m_infos[local_14].m_8);
            }
            else
            {
                local_24.put_byte(
                    static_cast<int>(static_cast<char>(local_18->m_infos[local_14].m_4)));
                local_24.put_byte(static_cast<unsigned int>(static_cast<unsigned char>(
                    local_18->m_infos[local_14].m_0)));
                local_24.put_byte(local_18->m_infos[local_14].m_8);
            }
            STSkillCommandData local_30;
            // [b28d] ORIG 0x081bf7d1 直接 test %al（无 cVar2b 具名 bool 槽）。
            if (user->findSkillCommandData(
                    static_cast<int>(static_cast<unsigned char>(
                        local_18->m_infos[local_14].m_0)),
                    local_30, true))
            {
                local_24.put_byte(1);
                // ORIG 0x081bf7e8 单条 movzbl+movsbl 直传（无 local_2e 槽）；
                // 0x081bf80a `movzbl -0x29(%ebp,%eax,1)` = 对象 +3 起字节数组视图。
                local_24.put_byte(static_cast<int>(static_cast<char>(local_30.m_count)));
                for (int local_10 = 0;
                     local_10 < static_cast<int>(static_cast<char>(local_30.m_count));
                     local_10 = local_10 + 1)
                {
                    local_24.put_byte(
                        static_cast<int>(static_cast<char>(local_30.m_keyBytes[local_10])));
                }
            }
            else
            {
                local_24.put_byte(0);
            }
        }
    }
    local_24.finalize(true);
    user->Send(local_24);
