bool SyncScript::getItemValueStirng(int ridableId, const BuyShopData& data,
                                    AdvanceAltarShopType::T type, std::string& out)
{
    const std::vector<BuyUpgradeData>* vec = 0;
    // ORIG 0x08134145..0x08134177 形：单次 `mov 0x10(%ebp),%eax` + cmp1/cmp2/
    // test 三连（4.4 **switch** lowering 特征；if-链会退化成 mem 操作数直比）。
    switch (type)
    {
    case AdvanceAltarShopType::T_1: vec = &data.m_vecT1; break;
    case AdvanceAltarShopType::T_2: vec = &data.m_vecT2; break;
    case AdvanceAltarShopType::T_0: vec = &data.m_vecT0; break;
    default: return false;
    }
    const int total = (int)vec->size();
    // 配方⑥（同 InsertDescTable）：ORIG i=0 在 beginEv(0x0813419c) **之后**
    // 0x081341a4 ⇒ 迭代器出环具名；配方⑦：ORIG 尾 `movl $0x0,0x8(%esp)`+ppEv
    // ⇒ postfix `it++`。
    std::vector<BuyUpgradeData>::const_iterator it = vec->begin();
    int i = 0;
    for (; it != vec->end(); it++, ++i)
    {
        std::string s;                 // ORIG 循环体内多余局部（汇编构造/析构实证）
        // ORIG 清零 rep stosl 0x100 dwords = **1024B**（0x081341bb..0x081341cf，
        // 缓冲 -0x444）⇒ conv 是 char[0x400]，非 0x100（帧差 0x85c-0x55c=0x300
        // 的唯一来源）。
        char conv[0x400] = {0};
        // 槽位证据：ORIG tuple 基址 -0x844 比 conv(-0x444) **更深** ⇒ 4.4
        // 后声明更深的分配律下 tuple 声明须在 conv 之后（循环体内）。
        char tuple[0x400];
        if (!CodePage::script2Database(
                const_cast<char*>(it->m_strDesc.c_str()), conv))
        {
            // 配方⑩：ORIG 先求值 c_str→%edi / database→%esi / script→%ebx，
            // **其后**才 C1 ⇒ 单语句临时。
            cMyTrace(
                "static bool advancealtar::SyncScript::getItemValueStirng(int, const advancealtar::BuyShopData&, advancealtar::AdvanceAltarShopType::T, std::tstring&)",
                0xd36, 5)(
                "Error!!! %s->%s [%s][%s][%d]",
                CodePage::script(), CodePage::database(),
                it->m_strDesc.c_str(),
                "static bool advancealtar::SyncScript::getItemValueStirng(int, const advancealtar::BuyShopData&, advancealtar::AdvanceAltarShopType::T, std::tstring&)",
                0xd36);
            return false;
        }
        sprintf(tuple, "(%d,%d,%d,'%s')", ridableId, (int)type,
                it->m_nItemId, conv);
        out += tuple;
        if (i < total - 1)
        {
            out += ", ";
        }
    }
    return true;
