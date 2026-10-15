// ---- Regenerate —— T 0x085f8646（Regenerate.md 全文转写）------------------
int RegenerationROI::Regenerate(int baseLv0, int baseLv1, int partIdx,
                                int optType, int& outCost, Inven_Item& outItem,
                                CUser* user)
{
    // [b28d-solo-RandomRentalA2 判词] DIFF 435/416（ORIG 0x085f8646）：P282/P48
    // 双 GetRandomOptionGrade 虚调已补齐（vptr+0x50 = _ZNK5CItem20GetRandomOptionGradeEv，
    // vtable word22 实证）；残余槽序/物化次数。旧判词：
    // （P183 族，大函数按 §6.2 三件套口径）。语义等价。
    std::stringstream ss(std::ios::out | std::ios::in);
    // [Lane13] P28/P331：ORIG 槽序 ss(-0x124) → rarity(-0x68)/optionGrade(-0x67)/
    // usableLevel(-0x66) → parts(-0x64) → sb(-0x58) ⇒ 三个窄日志临时须声明在
    // parts/sb 之前（4.4 依声明序占槽；原声明在函数尾部致整组槽位浅移 0x14）。
    signed char rarity;
    signed char optionGrade;
    unsigned short usableLevel;
    select_base_item sb;
    _RandomLevel(baseLv0, baseLv1, sb, ss);

    // 函数级 static handle（guard 0x09473100 / 对象 0x09473108）
    static random_option::CRandomOptionItemHandle handle;

    if (optType != 0 && optType != 0x65)
    {
        return 0x11;
    }

    outCost = _Calculate_RegenCost(partIdx, optType, baseLv0, baseLv1);
    if (outCost == -1)
    {
        // [Lane13] P221/P227：ORIG 085f878b 把 cMyTrace 构到 **-0x40**（浅槽），
        // 具名 `cMyTrace t` 被 4.4 放最深 -0x6c 并顶浅整组对象槽 ⇒ 改全表达式
        // 临时链（构造即调用），复现 ORIG 槽深。
        cMyTrace(__PRETTY_FUNCTION__,
                 0x47, 5)("ROI regen Error : %d, %d, %d, %d", partIdx,
                          optType, baseLv0, baseLv1);
        return 0x11;
    }

    std::vector<int> parts;
    if (partIdx != 0)
    {
        _Select_Part(partIdx, parts);
    }
    if (partIdx == -1)
    {
        return 0x11;
    }

    int chosenIdx = (int)handle.RegenVer_choose_item((ENUM_RARITY)2, sb.m_level0,
                                                     parts);
    // [b28d-solo-RandomRentalA2] P167：ORIG 日志重载 = _ZNSolsEm(ulong)/
    // _ZNSolsEi(int)/_ZNSolsEt(ushort)/_ZNSolsEi(int) —— 逐值对齐重载。
    ss << "base item index : " << (unsigned long)chosenIdx << "\n";

    CItem* base = G_CDataManager()->find_item(chosenIdx);
    if (base == 0)
    {
        ss << "base item is Null\n";
        SendMessageToCUser(user, ss.str().c_str());
        return 0x11;
    }

    // [SweepRandomRental 修正] ORIG 0x085f890e/085f891c/085f8935 三个日志局部
    // 为窄类型：`mov %al,-0x68`(rarity char) / `mov %ax,-0x66`(usableLevel ushort)
    // / `mov %al,-0x67`(optionGrade char)，打印时 movzbl+movsbl / movzwl 扩展
    // ⇒ 原为 char/unsigned short/char（旧 int 形在值域溢出时日志值与 ORIG 不同）。
    // [Lane13] 声明已前置到 ss/sb 之前（槽序对齐），此处仅赋值。
    rarity = (signed char)base->get_rarity();
    usableLevel = (unsigned short)base->getUsableLevel();
    optionGrade = (signed char)base->GetRandomOptionGrade();  // vtable 槽20（+0x50）

    ss << "rarity : " << (int)rarity << " level : " << usableLevel
       << " base item item : " << (int)optionGrade << "\n";

    outItem.reset();
    outItem.m_addInfo = chosenIdx;
    G_CDataManager()->m_itemList->create_item(ENUM_ITEM_CREATE_TYPE_0, outItem,
                                              1);

    // [SweepRandomRental 修正] ORIG 0x085f8a77 `xor $0x1,%eax; test %al,%al;
    // je <成功块>` = `if (!call()) { 失败; return; }`（bool callee 直测，无 ok
    // 具名槽；旧 `bool ok` 形多 2 条且块序相反）。
    if (!handle.RegenVer_give_option(
            // [b28d-solo-RandomRentalA2] P48：ORIG 在此处重新调用 get_rarity/
            // getUsableLevel（不用上方日志用的缓存局部），实参求值序右→左。
            (unsigned long)chosenIdx, (ENUM_RARITY)base->get_rarity(),
            base->getUsableLevel(), base->getItemGroupName(),
            base->GetRandomOptionGrade(), &outItem.m_random.m_random, this,
            optType))
    {
        ss << "give_option false\n";
        SendMessageToCUser(user, ss.str().c_str());
        return 0x13;
    }

    for (int i = 0; i < 3; ++i)
    {
        // [SweepRandomRental] P183：ORIG 每处重算 `&outItem + i*3 + 0x25`
        // （0x085f8bde `mov i; mov &outItem; add; add; lea; add $0x25`），
        // 不缓存 slot 指针（旧 slot 局部多占一槽且形不符）。
        ss << "[" << i << "] :"
           << (int)outItem.m_random.m_random.m_fields[i].m_optionIndex
           << ":"
           << (int)outItem.m_random.m_random.m_fields[i].m_firstValue
           << ":"
           << (int)outItem.m_random.m_random.m_fields[i].m_secondValue
           << "\n";
    }
    SendMessageToCUser(user, ss.str().c_str());
    return 0;
}