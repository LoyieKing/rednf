void Inter_LoadInven::_UpdateRentalItems(CUser* user, SIG_LOAD_INVENTORY* sig)
{
    if (user == 0 || sig == 0)
        return;

    const int channelType = G_GameWorld()->GetChannelType();
    if (channelType == 0xf || channelType == 0x10)
    {
        user->DeleteRentalItemInfo(1);
        return;
    }

    InstanceRentalSystem::RentInput input;
    input.m_machineKey = 1;
    input.m_logFlag = 1;
    // P324：ORIG 084c45cf `test %al,%al; je 084c4680` ⇒ 原码是**正向** Check（为真走
    //   exchange 路径，物理在前）；我方原写的 `!Check` 会多出 `sete` 形态差。
    if (user->CheckLoadRentalInfoFromExchange())
    {   // exchange 表：msg + i*0xc + 0x153e4，仅本轮 1 条；{int!=0, byte4, byte5,
        // byte6(bEquip), int8}（ORIG 逐字节 movzbl 读）；elem 的 ctor 084c45db 在循环外
        //   只调一次（-0x3c）。
        InstanceRentalSystem::RentInElem elem;
        for (int i = 0; i < 1; ++i)
        {
            char* p = reinterpret_cast<char*>(sig) + i * 0xc + 0x153e4;
            if (*reinterpret_cast<int*>(p) == 0)
                continue;
            elem.m_sectionIdx = p[4];                   // 084c4612 movzbl 0x4(%eax)
            elem.m_itemIdx = p[5];                      // 084c461c movzbl 0x5(%eax)
            elem.m_bEquip = 0;                          // 084c4623 movb $0x0,-0x3a(%ebp)
            // 084c4627..084c4644：sig 与 i 重算地址（不复用 p）取 +0xa，形态是
            //   「置 0 + 条件置 1」而不是三元 setne；ORIG 全程未写 elem+3(type)。
            if (reinterpret_cast<char*>(sig)[i * 0xc + 0x153ea] != 0)
                elem.m_bEquip = 1;                      // 084c4646 movb $0x1,-0x3a(%ebp)
            elem.m_rentItemId = *reinterpret_cast<int*>(p + 8);   // 084c464d mov 0x8(%eax)
            input.m_elems.push_back(elem);
        }
    }
    // 084c4680：Check 为假 → vector 路径（-0x48 vector / -0x4c iterator / -0x54 elem）。
    else
    {
        std::vector<InstanceRentalSystem::RentalInfo> rentalList;
        if (user->GetCurrentCharacRentalInfo(rentalList, 1))
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
                elem.m_bEquip = 0;                          // 084c46f1 movb $0x0,-0x52(%ebp)
                // 084c46f8：ORIG 只读 r[0xa]，形态是「置 0 + 条件置 1」（非三元 setne）；
                // ORIG 全程未写 elem+3(type)。
                if (r.m_inEquip != 0)
                    elem.m_bEquip = 1;                      // 084c4700 movb $0x1,-0x52(%ebp)
                elem.m_rentItemId = r.m_rentItemId;         // 084c4707 mov 0xc(%eax)
                input.m_elems.push_back(elem);
            }
        }
    }

    if (input.m_elems.empty())
    {
        user->DeleteCurrentCharacRentalItemInfo(1);
    }
    else
    {
        InstanceRentalSystem::RentOutput output;
        const int rentRet =
            static_cast<int>(GlobalData::s_rentalSystem->Rent(*user, input, output));
        if (rentRet == 0)
            user->UpdateCurrentCharacRentalInfo(output, 1, true);
    }
