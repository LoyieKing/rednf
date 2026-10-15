// ---- give_option —— T 0x085f2cc6（ORIG 独立完整实现，逐段转写）----------
// 注意：辅助调用参数顺序按 ORIG 反汇编实证——
//   _get_option_type 第4参=grade（内层 find 键，非 group）；
//   _get_option_quantity 键=(rarity,ff,outGrade,level)，回退 b1=grade；
//   _choose_option_group 键=(rarity,ff,group,level)+u32(outGrade)。
// [RentalPrett 2026-09-11] 首参拼写改 typedef 名 `itemIndex_t`（GameTypes.h，= unsigned long；
//   mangled 仍 `…11give_optionEm…` 不变）⇒ __PRETTY_FUNCTION__ 按**定义处**拼写展开，
//   与 ORIG prett 串 `bool random_option::CRandomOptionItemHandle::give_option(itemIndex_t,
//   ENUM_RARITY, int, int, int, RandomOption*)` 逐字节一致（同 TU 先例：change_option P139）。
bool CRandomOptionItemHandle::give_option(itemIndex_t itemIdx,
                                          ENUM_RARITY rarity, int level,
                                          int group, int grade,
                                          RandomOption* opts)
{
    // [b28d-solo-RandomRentalA2 判词] DIFF 434/430（ORIG 0x085f2cc6）：P0/P48 四处
    // GetRandomOption 内联已归位；残余槽序/物化次数。旧判词：
    // （P183 族）。语义等价。
    unsigned char outGrade = 0;
    int err = 0;
    err = _get_option_type(itemIdx, rarity, level, grade, outGrade);
    // [K2ERandomBoss 2026-09-11 P225] ORIG 0x085f2d05（call _get_option_type）后段为
    // `mov %eax,-0x1c; cmpl $0x0,-0x1c; setne %al; test %al,%al; je <续>`（4.1
    // 槽比零 bool 物化）；探针 /tmp/rr_probe_setne.cpp fB 实证 `while (e != 0)
    // { body; }` 复现同形（4.4 对 if 形恒折直跳）。body 末尾 return ⇒ 语义等价。
    while (err != 0)
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            __PRETTY_FUNCTION__,
            0x2f9, "_get_option_type (%d,%d,%d,%d,%d) error_line(%d)",
            (int)itemIdx, (int)rarity, level, grade, (int)outGrade, err);
        return false;
    }

    int quantity = 0;
    err = _get_option_quantity(rarity, grade, outGrade, level, quantity);
    // [K2ERandomBoss 2026-09-11 P225] 同 _get_option_type 处：ORIG 槽比零 setne/test
    // 物化形（4.4 对 if 形恒折直跳，见 /tmp/rr_probe_setne.cpp；body 末尾 return ⇒ 等价）。
    while (err != 0)
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            __PRETTY_FUNCTION__,
            0x300, "_get_option_quantity (%d,%d,%d,%d,%d) error_line(%d)",
            (int)rarity, grade, (int)outGrade, level, quantity, err);
        return false;
    }

    // ORIG asm 85f2e27：m_field10(+0x10) 非零时覆盖数量（RegenVer 无此步）。
    if (m_field10 != 0)
        quantity = m_field10;

    int groupId = 0;
    err = _choose_option_group(rarity, grade, group, level, outGrade, groupId);
    // [K2ERandomBoss 2026-09-11 P225] 同上（ORIG 0x085f2e75 调用点后段 setne/test 物化形）。
    while (err != 0)
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            __PRETTY_FUNCTION__,
            0x30d, "_choose_option_group (%d,%d,%d,%d,%d,%d) error_line(%d)",
            (int)rarity, grade, group, level, (int)outGrade, groupId, err);
        return false;
    }

    // 不同权重表：map<short, DifferentWeightValue> @+0xf4；
    // 键 short = 低字节 rarity | 高字节 outGrade（asm -0x6e/-0x6d 实证）。
    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 4 处独立调 GetRandomOption
    // （mapf4 find/end + map160 find/end），无具名 script 槽。
    DifferentWeightValue dwTmp;  // ORIG 未使用临时（O0 保留 ctor call）
    unsigned short dwRaw =
        (unsigned short)(((unsigned short)(unsigned char)rarity) |
                         ((unsigned short)outGrade << 8));
    std::map<short, DifferentWeightValue>::const_iterator dw =
        reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapf4.find((short)dwRaw);
    if (dw == reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapf4.end())
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            __PRETTY_FUNCTION__,
            0x319, "different_weight_ (%d,%d) ",
            (int)rarity, (int)outGrade);
        return false;
    }
    // [R9form4 2026-09-12 形态对齐] ORIG 0x085f2cc6 段：单次 `ptEv` 后整体拷贝 8 字节子对象
    //   （`mov 0x8(%eax),%edx; mov 0x4(%eax),%eax; mov %eax,-0x78; mov %edx,-0x74`）；
    //   旧形两次 `dw->second.m_*` 独立访问 ⇒ 两次 ptEv（p167 EXTRA ×1）。
    DifferentWeightValue dwV = dw->second;

    // 组值表：map<int, OptionGroupValue> @+0x160。
    std::map<int, OptionGroupValue>::const_iterator ogIt =
        reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_map160.find(groupId);
    if (ogIt == reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_map160.end())
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            __PRETTY_FUNCTION__,
            0x321, "option_group_ (%d) ", groupId);
        return false;
    }

    int remaining = ogIt->second.m_fieldC;
    std::vector<OptionGroup> groupVec = ogIt->second.m_vec0;  // 值拷贝后 erase
    // [Lane13 视图 cast 反模式] 原 `unsigned char* optB = (unsigned char*)opts` +
    // `optB[slotI*3+k] = (unsigned char)v` 使 4.4 折 LHS-first/窄化中转；改权威头
    // 具名字段直访问（RandomOption::m_fields[i] 3 字节组，偏移零变化）。

    // [Lane17 Q1 判据⑫ 已试并回退] ORIG clr_l=6（-0x1c/-0x68/-0x6c/-0x14/-0x10/-0xc）
    // vs 我方 4；按「缺具名 int 局部」补 `int fieldIdx = 0;` + 循环末 `fieldIdx = fieldIdx + 1;`
    // 后实测**反劣**：ours 435→438（gap −1→−4），我方 clr 4→5 但仍缺 1。
    // ⇒ ORIG 的 -0x10/-0xc 两槽对应的是**循环变量 i 与 sel 的具名声明**
    // （我方 slotI 单变量已同时承担两者、同步递增，补独立索引纯增冗余指令）。
    // 判据⑫ 在本件不成立，已回退；give_option 残余维持 P292 族判词。
    // [R28fmt 2026-09-13 §27.4 求值次数/支配域] ORIG 的 iterator 默认构造
    // （`_ZN9__gnu_cxx17__normal_iteratorIP11OptionGroupSt6vectorIS1_SaIS1_EEEC1Ev`
    // @0x085f309d）发射在 for-init（`movl $0x0,-0xc` @0x085f30a2）**之前** = 循环外
    // （回边 0x085f32a0→0x085f30ae）；我方原先声明在 if 块内 ⇒ C1Ev 每轮在回边区发射。
    // 声明上提后与 ORIG 同点（默认构造为平凡置 NULL，语义不可观察，仅求值次数/域对齐）。
    std::vector<OptionGroup>::iterator victim;
    for (int slotI = 0; slotI < quantity && slotI <= 2; ++slotI)
    {
        unsigned long bound = (unsigned long)remaining;
        int pickN = m_rand[9].randInt(bound);      // m_rand+0x5808 → 池[9]

        // [Lane13] P278/P221：chooser 匿名临时，get_value 作用 for_each 返回值。
        unsigned int sel =
            std::for_each(groupVec.begin(), groupVec.end(),
                          CRandomList<unsigned int>((unsigned int)pickN))
                .get_value();
        if (sel < groupVec.size())
        {
            victim = groupVec.begin() + sel;
            // [K2ERandomBoss 2026-09-11] P167 差集：ORIG 2×`iterator::deEv`（同一
            // victim 解引用两次）+ 无 `vector::ixEj`；原 `OptionGroup og = groupVec[sel]`
            // 产 ixEj 且少 2 处 deEv。改为 `(*victim).m_fieldN` 直读同一迭代器。
            opts->m_fields[slotI].m_optionIndex = (unsigned char)(*victim).m_type0;  // 槽首字节=组类型
            remaining -= (*victim).m_weight4;
            groupVec.erase(victim);

            opts->m_fields[slotI].m_firstValue =
                (unsigned char)_get_option_value(dwV.m_field0, dwV.m_field4);
            opts->m_fields[slotI].m_secondValue =
                (unsigned char)_get_option_value(dwV.m_field0, dwV.m_field4);
        }
    }

    // seed 字节(+9)：低 2 位 = outGrade；高位 = 池[10](+0x61d0) 掷 0x40 后 <<2
    opts->m_gradeSeed.m_value =
        (unsigned char)((opts->m_gradeSeed.m_value & 0xfc) | (outGrade & 3));
    unsigned long seedBound = MAX_RANDOM_SEED;
    // [R16a07] 删 `int seedRoll = ...` 具名局部、randInt 内联进实参位（P0：ORIG
    // 0x85f32e5 `call randInt; mov %eax,%edx` 无槽往返，我方原发
    // `mov %eax,-0x18(%ebp)` + `mov -0x18(%ebp),%eax` 一对；掩码 `& 0x3f` 照旧保留）。
    opts->m_gradeSeed.m_value =
        (unsigned char)((opts->m_gradeSeed.m_value & 3) |
                        ((m_rand[10].randInt(seedBound) & 0x3f) << 2));

    return true;
}