int CRandomOptionItemHandle::_choose_option_group(ENUM_RARITY rarity, int level,
                                                  int grade, int optType,
                                                  int group, int& outGroup)
{
    // [b28d-solo-RandomRentalA2 判词] AE（[Lane13 2026-09-10] 已归零；原 DIFF 193/172；ORIG 0x085f2882）：P0/P183
    // 4 处 provider 虚调内联已归位；残余槽序/物化次数。旧判词：
    // （P183 族）。语义等价。
    // [SweepRandomRental-2 P82] ORIG 0x085f28a1 `mov 0x1c(%ebp),%eax;
    // mov %eax,-0x58(%ebp)` 为单条 disp 写 ⇒ 键是具名 8 字节 POD 视图
    // （`*(unsigned int*)(keyBytes+4)` 生 lea+lea+mov (%edx) 三条）。
    union OptionGroupKey
    {
        struct
        {
            unsigned char m_rarity;    // +0x0
            unsigned char m_level;     // +0x1（0xff 或 level）
            unsigned char m_grade;     // +0x2
            unsigned char m_optType;   // +0x3
            unsigned int m_group;      // +0x4
        } m_parts;
        unsigned long long m_key;      // 8 字节键视图（find 直传左值）
    };
    OptionGroupKey key;
    key.m_parts.m_rarity = (unsigned char)rarity;
    key.m_parts.m_level = 0xff;
    key.m_parts.m_grade = (unsigned char)grade;
    key.m_parts.m_optType = (unsigned char)optType;
    key.m_parts.m_group = (unsigned int)group;

    // [b28d-solo-RandomRentalA2] P0/P183：ORIG 4 处独立重取 m_optionSet 并虚调
    // get_option_group_choice()（find/end ×2），无 provider/choiceMap 具名局部。
    // ORIG find 直传 &键区 8 字节引用（无中间 u64 拷贝）。
    // [b28d-solo-RandomRentalA2 P96] ORIG 把非 const `find()` 结果赋给
    // const_iterator（p167 MISSING `C1ERKSt17_Rb_tree_iterator` 转换构造 +
    // `_ZNKSt23_Rb_tree_const_iterator...ptEv/eq` + const vector begin/end/for_each/at）
    // ⇒ 保持非 const provider，仅把 it 声明为 const_iterator。
    std::map<unsigned long long, OptionGroupChoiceValue>::const_iterator it =
        static_cast<CRandomOptionGive*>(m_optionSet)
            ->get_option_group_choice()
            .find(key.m_key);
    if (it == static_cast<CRandomOptionGive*>(m_optionSet)
                  ->get_option_group_choice()
                  .end())
    {
        key.m_parts.m_level = (unsigned char)level;
        // [SweepRandomRental-2 P293 反向] ORIG 0x085f2940..0x085f29d2 为
        // 虚调#3+find（→-0x48，const_iterator ctor →-0x64）**再** 虚调#4+end
        // （→-0x40，ctor →-0x44），比较 (find,end)；单表达式 `find()==end()`
        // 令 4.4 反序成 end 先/find 后 ⇒ 具名 const_iterator 形。
        // [b28d-solo-RandomRentalA2 P171] 第二次 find 不回写外层 it（后续仍用
        // 首次迭代器）。
        std::map<unsigned long long, OptionGroupChoiceValue>::const_iterator retryIt =
            static_cast<CRandomOptionGive*>(m_optionSet)
                ->get_option_group_choice()
                .find(key.m_key);
        if (retryIt == static_cast<CRandomOptionGive*>(m_optionSet)
                           ->get_option_group_choice()
                           .end())
        {
            return 0x29e;
        }
    }

    // [SweepRandomRental-2 P183] ORIG 每处重取 it->second（ptEv + 偏移折叠：
    // m_field18→+0x20、m_vecC→+0x14、m_vec0→+0x8），无 OptionGroupChoiceValue&
    // 缓存（旧形 add $0x8 落 -0x14 槽并多次回读）。
    // [b28d-solo-RandomRentalA2 P0] bound 匿名临时形。
    int r = m_rand[6].randInt((unsigned long)(it->second.m_field18 - 1));

    // [Lane13] P278/P221：同族——chooser 匿名临时，get_value 作用 for_each 返回值。
    unsigned int sel =
        std::for_each(it->second.m_vecC.begin(), it->second.m_vecC.end(),
                      CRandomList<unsigned int>((unsigned int)r))
            .get_value();
    // [SweepRandomRental-2] ORIG 0x085f2ac5 `cmp -0xc(%ebp),%eax; seta %al`
    // ⇒ 源码正极性 `size() > sel`（`sel >= size()` 早返形产 setbe + 反块序）。
    if (it->second.m_vec0.size() > sel)
    {
        outGroup = it->second.m_vec0.at(sel);
        return 0;
    }
    return 0x2be;
