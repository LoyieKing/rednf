int CRandomOptionItemHandle::RegenVer_get_option_quantity(
    ENUM_RARITY rarity, int level, int grade, int optType, int& outQuantity,
    RegenerationROI* roi)
{
    // [b28d-solo-RandomRentalA2 判词] AE（[Lane13 2026-09-10] 已归零；原 DIFF 99/112；ORIG 0x085f1a62）：P0/P48 四处
    // GetRandomOption 内联已归位；残余槽序/物化次数。旧判词：
    // 语义等价。
    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 4 处独立调 GetRandomOption
    // （两次 find + 两次 end），无具名 script 槽。
    // [SweepRandomRental-2 P82] 同 _get_option_quantity：具名 4 字节 POD 视图
    // （数组形使 4.4 把 &key 缓存进 %ebx，键地址装载序亦异）。
    union OptionQuantityKey
    {
        struct
        {
            unsigned char m_rarity;    // +0x0
            unsigned char m_level;     // +0x1（0xff 或 level）
            unsigned char m_grade;     // +0x2
            unsigned char m_optType;   // +0x3
        } m_parts;
        int m_key;                     // 4 字节键视图（find 直传左值）
    };
    OptionQuantityKey key;
    key.m_parts.m_rarity = (unsigned char)rarity;
    key.m_parts.m_level = 0xff;
    key.m_parts.m_grade = (unsigned char)grade;
    key.m_parts.m_optType = (unsigned char)optType;

    // [b28d-solo-RandomRentalA2 P0/P235] 同 _get_option_quantity：去 memcpy。
    std::map<int, OptionQuantityValue>::const_iterator it =
        reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapb4.find(
            key.m_key);
    if (it == reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapb4.end())
    {
        key.m_parts.m_level = (unsigned char)level;
        // [SweepRandomRental-2 P293 反向] ORIG 先 find 再 end（同
        // _get_option_quantity）⇒ 具名 const_iterator。
        // [b28d-solo-RandomRentalA2 P171] 第二次 find 的结果只用于与 end
        // 比较、**不回写 it**（Ghidra: find(local_20) vs end(local_10)，后续
        // operator-> 仍取 local_1c=首次迭代器）⇒ 照抄原码遗留行为。
        std::map<int, OptionQuantityValue>::const_iterator retryIt =
            reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapb4.find(
                key.m_key);
        if (retryIt ==
            reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapb4.end())
        {
            return 0x1ad;
        }
    }

    if (it->second.m_maxB == 0)
    {
        return 0x1b1;
    }

    // [Lane13] P211：ORIG 085f?? `call; mov %eax,%edx; mov 0x1c(%ebp),%eax;
    // mov %edx,(%eax)` —— RHS 与 int 目标符号性异型 ⇒ RHS-first + eax→edx 中转；
    // 显式 `(unsigned int)` cast 复现该异型（同族见 _get_option_quantity）。
    outQuantity = (unsigned int)roi->RegenVer_get_option_quantity(
        it->second.m_minA, it->second.m_maxB);
    if (outQuantity == 0)
    {
        return 0x1b5;
    }
    return 0;
