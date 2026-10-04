// ORIG T _ZN15Inter_LoadInven24_UpdateDimensionRentalItemsEP5CUserP18SIG_LOAD_INVENTORY
// @0x084c4858（报告 _UpdateDimensionRentalItems.md 两段全读）：维度租借机器键 2、
// exchange 表 0xb 条 stride 8（msg+(i+0x2a7e)*8）、elem.type 取自表第 7 字节或
// RentalInfo+0xb；其余与 PC 房租借同构。m_rentItemId 表路径无数据，按 ORIG
// RentInElem ctor 0x0822f02e 置 -1 [推断：movl $0xffffffff 实证]。
// [b28d-lane2] verdict=DIFF ndiff=194（ORIG 0x084c4858）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-18。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84c4858）：本轮 P167/P0 具名引用 r=*it 单次 de+字段回读（p167 CLEAR，pt×5→de×1）；余 N18a/N82/P112 帧槽。
void Inter_LoadInven::_UpdateDimensionRentalItems(CUser* user,
                                                  SIG_LOAD_INVENTORY* sig)
{
    if (user == 0 || sig == 0)
        return;

    const int channelType = G_GameWorld()->GetChannelType();
    if (channelType == 0xf || channelType == 0x10)
    {
        user->DeleteRentalItemInfo(2);
        return;
    }

    InstanceRentalSystem::RentInput input;
    input.m_machineKey = 2;
    // P324：ORIG 084c48c7 `test %al,%al; je 084c495f` ⇒ 原码是**正向** Check（为真走
    //   exchange 路径，该路径物理在前）；我方原写的 `!Check` 会多出 `sete` 形态差。
    if (user->CheckLoadRentalInfoFromExchange())
    {   // exchange 表：msg + (i+0x2a7e)*8，i<=0xa；{int!=0, byte4, byte5, byte6(bEquip),
        // byte7(type)}（ORIG 逐字节 movzbl 读）；elem 的 ctor 084c48d3 在循环外只调一次。
            InstanceRentalSystem::RentInElem elem;
        for (int i = 0; i <= 0xa; ++i)
        {
            const SIG_SAVE_DIMENSION_RENT_INFO* p = &sig->m_rentDimension.m_entry[i];
            if (p->m_itemIndex == 0)
                continue;
            elem.m_sectionIdx = p->m_field4;
            elem.m_itemIdx = p->m_field5;
            elem.m_bEquip = 0;
            elem.m_type = p->m_field7;
            if (p->m_inEquip != 0)
                elem.m_bEquip = 1;
            input.m_elems.push_back(elem);
        }
    }
    // 084c495f：Check 为假 → vector 路径。ORIG 全程无 m_rentItemId 赋值（只写 elem+0..+3），
    //   该字段值来自 ctor 0x0822f02e（elem+4 = -1）⇒ 不写该字段。
    else
    {
        std::vector<InstanceRentalSystem::RentalInfo> rentalList;
        if (user->GetCurrentCharacRentalInfo(rentalList, 2))
        {
            for (std::vector<InstanceRentalSystem::RentalInfo>::iterator it =
                     rentalList.begin();
                 it != rentalList.end(); ++it)
            {
                // P167/P0：ORIG 单次 operator*（de）→ 槽往返 → 4 字段回读 ⇒ 具名引用局部。
                const InstanceRentalSystem::RentalInfo& r = *it;
                InstanceRentalSystem::RentInElem elem;      // ← RentalInfo+8..+0xf
                elem.m_sectionIdx = r.m_field8;             // +0x08
                elem.m_itemIdx = r.m_field9;                // +0x09
                elem.m_bEquip = 0;                          // 084c49d0 movb $0x0,-0x52(%ebp)
                elem.m_type = r.m_fieldB;                   // +0x0b
                if (r.m_inEquip != 0)
                    elem.m_bEquip = 1;                      // 084c49e9 movb $0x1,-0x52(%ebp)
                input.m_elems.push_back(elem);
            }
        }
    }

    // [G18 T16 2026-09-11 · P354 反向（ORIG 有 xor）实证：负结论，已还原]
    // 试「交换 if/else 体 + 条件取反」（`if (!input.m_elems.empty()) { rent… } else
    // { delete }`）：**if/else 块序与 xor 极性确已对齐**（该 hunk 从 diff 消失），
    // 但 our 219→220 ⇒ gap −3→**−4**（净劣 1 条）⇒ 按「gap 严格改善」闸门还原原形。
    // 同构件 `_UpdateRentalItems`（PC 房版）据同族结论不再单独试。
    if (input.m_elems.empty())
    {
        user->DeleteCurrentCharacRentalItemInfo(2);
    }
    else
    {
        InstanceRentalSystem::RentOutput output;
        const int rentRet =
            static_cast<int>(GlobalData::s_rentalSystem->Rent(*user, input, output));
        if (rentRet == 0)
            user->UpdateCurrentCharacRentalInfo(output, 2, true);
    }
}