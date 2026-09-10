int Dispatcher_PcRoomRentItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stRentItemParam& out = static_cast<stRentItemParam&>(param);
    MSG_RENT_ITEM& req = reinterpret_cast<MSG_RENT_ITEM&>(msg);

    out.m_4 = check_error(user, msg);
    if (out.m_4 != 0)
    {
        return 0;
    }

    InstanceRentalSystem::RentInput input;               // ctor 清 vector/machineKey
    input.m_logFlag = 1;                                 // +0x10 log 开关（PcRoom 报告实证）
    input.m_machineKey = static_cast<int>(static_cast<char>(req.m_0e));
    // ORIG 0x081e52c0：两读取为 `movzbl 0xf(%eax,%edx,2)` / `movzbl 0x10(%eax,%edx,2)`
    // —— 基址=msg(-0x10)、**缩放因子 2**、disp 0xf 与 0x10 ⇒ 原码是 msg+0xf 起的
    // 2 字节元素结构数组（uchar 对），非 `char[]` 的 `i*2`/`i*2+1` 下标形
    // （后者 4.4 恒产 lea+×1，P311 成员数组视图判据）。MSG 头冻结 ⇒ 本 TU 视图承接。
    struct __attribute__((packed)) RentPairView
    {
        char m_head[0xf];
        struct Pair { unsigned char m_sectionIdx, m_itemIdx; } m_pairs[1];  // +0xf 起
    };
    // 视图指针**不落槽**（ORIG 无该槽，P0）⇒ 每使用点内联 cast 表达式。
    for (int i = 0; i < static_cast<int>(static_cast<char>(req.m_0d)); ++i)
    {
        InstanceRentalSystem::RentInElem elem;
        elem.m_sectionIdx =
            reinterpret_cast<RentPairView*>(&msg)->m_pairs[i].m_sectionIdx;
        elem.m_itemIdx =
            reinterpret_cast<RentPairView*>(&msg)->m_pairs[i].m_itemIdx;
        input.m_elems.push_back(elem);
    }

    giveDimensionItems(user, input);

    out.m_4 = static_cast<int>(
        static_cast<InstanceRentalSystem*>(GlobalData::s_rentalSystem)
            ->Rent(*user, input, out.m_output));
    if (out.m_4 == 0)
    {
        user->UpdateCurrentCharacRentalInfo(
            out.m_output, (int)(char)req.m_0e, false);
    }
    return 0;                                            // ORIG RentInput dtor 于帧尾
