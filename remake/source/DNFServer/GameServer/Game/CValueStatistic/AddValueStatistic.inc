bool CValueStatistic::AddValueStatistic(VALUE_STATISTIC_FIELD field, CUser* user,
                                        unsigned int value)
{
    // [b28d-59] 封 N82/N93（ORIG 0x0860d682）：已还原——无 fieldIdx/level 具名局部（P0：形参直读 +
    // 索引处内联 get_charac_level()，0x0860d7a5 每次重调）；条件为
    // field<0(js) / field>0x1d(jg) / level<=0(jle) / level>0x46(jle) 四项短路，
    // 汇聚 bool 后 `test %al; je`。插入路径也是 `+= value`（读-加-写，非直接赋值）。
    if (field < 0 || field > 0x1d ||
        user->get_charac_level() <= 0 || user->get_charac_level() > 0x46)
    {
        // [b28d-59] 全表达式临时链（P227）：ORIG 0x0860d6d2 在 cMyTrace ctor 之前
        // 求 varargs（get_charac_level → %ebx、field → %esi 跨 ctor 保活），具名
        // trace 会把它挪到 ctor 之后。实参序 (field, level)。
        cMyTrace(__PRETTY_FUNCTION__, 0x306, false)(
            "VALUE_STATISTIC wrong index (%d,%d)",
            field, user->get_charac_level());
        return false;
    }

    // [A24TailBatch2] P236/P0：ORIG 0x0860d6a3 `call GetServerGroup; mov %eax,-0x42c8(%ebp);
    // lea -0x42c8(%ebp),%ecx`（匿名临时槽绑定 const&，非具名 group 深槽）⇒ find 实参内联。
    // 旧具名 group 被 4.4 排入深区（-0x42d8）挤占 cMyTrace 槽位（ORIG cMyTrace@-0x42d8）。
    std::map<ENUM_SERVER_GROUP, stValueStatistic>::iterator it =
        m_data.find(user->GetServerGroup());
    // [A24TailBatch2] 判据①：ORIG 0x0860d788 `call ne; test %al; je 0860d7e8` ⇒
    // ne 为假（it==end）跳插入块 ⇒ 原码是 `if (it != end()) { 更新 } else { 插入 }`
    // （fall-through=更新 0x10f..0x161、je 目标=插入 0x166）；旧写 `== end()` 使
    // 4.4 反转块序（je→更新 / fall-through=插入）⇒ POLARITY 差。
    if (it != m_data.end())
    {
        // [A24TailBatch2] P0：ORIG 0x0860d79a `call ptEv; add $0x4,%eax; mov %eax,-0x1c(%ebp)`
        // ⇒ 原码有具名引用局部（槽往返），两次使用均 `mov -0x1c(%ebp),%ecx` 重读。
        stValueStatistic& stat = it->second;
        stat.m_values[user->get_charac_level()][field] += value;
    }
    else
    {
        // [A24TailBatch2] ORIG 0x0860d801 直接 `call memset`（非 reset() 转发；
        // 实参 lea stat / 0 / 0x2148）+ 0x0860d822 **纯 store**（无 load-add）⇒
        // 插入路径是 `= value`（旧 `+=` 多 5~6 条重算索引+读改写）。
        stValueStatistic stat;
        memset(&stat, 0, sizeof(stat));
        stat.m_values[user->get_charac_level()][field] = value;
        // [A24TailBatch2] ORIG 0x0860d82f 插入路径**再调一次** GetServerGroup 且结果落
        // 独立临时槽 -0x20（0x0860d806 索引用、0x0860d82f make_pair 用）⇒ 原码写
        // `make_pair(user->GetServerGroup(), stat)`（右值实参 ⇒ 引用绑定临时槽），
        // 而非复用前面已具名的 group（旧形少 3 条 call 序列 + 少一槽）。
        m_data.insert(std::make_pair(user->GetServerGroup(), stat));
    }
    return true;
