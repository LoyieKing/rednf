// ORIG 0x0860daaa：无 user 判空、无 level 局部缓存（get_charac_level 在条件里被
// 调两次、错误分支再调一次、命中分支各调一次），且槽位不是 map::operator[] 而是
// find/end 比较 + 未命中时栈上零初始化记录 insert(make_pair(...))。
// [M4FreeNext15C.M4S1 round5] 形归位后 150/150（残余仅寄存器形，两处同数量）：
//   · 条件段 ORIG 0x0860daf0 `mov $1,%eax / jmp +0x46 / mov $0,%eax / test %al,%al
//     / je 0860db52(+0xa8)` —— 探针实证：含调用的短路链**直接**作 if 条件即产此形
//     （赋给具名 bool 才落 `mov %al,-0x19; cmpb` 栈槽，旧判「P0/P168a③b 不可翻」推翻）。
//   · 命中分支 ORIG 0x0860dbcd `call ptEv; add $0x4,%eax; mov %eax,-0x1c(%ebp)` 具名
//     `&it->second` 物化槽，读/写各重载一次（我方旧形用 `+4` disp 折平，少 3 条）。
//   · 残余 2 处皆为寄存器选择（读回 `mov (%ebx,%eax,4),%eax` 后 ORIG `mov %eax,%ebx;
//     add value,%ebx` vs 我方 `add value,%eax; mov %eax,%ebx`；insert 分支 ORIG
//     `lea (%eax,%ecx,1),%edx` 双层索引 vs 我方 `add %ebx,%eax` 单层）——同数量、同语义。
bool CCirculationStatistic::AddCirculationStatistic(CIRCULATION_STATISTIC_FIELD field, CUser* user,
                                                       unsigned int value)
{
    // [M4FreeNext15C.M4S1 round5 归位] ORIG 0x0860daaa `mov $1,%eax / jmp +0x46 /
    // mov $0,%eax / test %al,%al / je 0860db52` —— 0/1 物化留在 %al 直测、无栈槽。
    // 旧判「bool 槽 `mov %al,-0x19; cmpb` 不可翻」已被探针推翻：4.4 对**含调用的
    // 短路链直接作 if 条件**（探针 b1/b2/b4/b7 四形）即产该形；一旦把表达式赋给
    // 具名 bool（b8）才落栈槽。⇒ 删具名 wrongIndex 内联回 if 条件。
    if (!(field >= 0 && field < 0x30 &&
          user->get_charac_level() > 0 &&
          user->get_charac_level() < 0x47))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x39b, 0)(
            "CIRCULATION_STATISTIC wrong index (%d,%d)", field, user->get_charac_level());
        return false;
    }

    std::map<ENUM_SERVER_GROUP, stCirculationStatistic>::iterator it =
        m_data.find(user->GetServerGroup());
    // [L6-S1 P167/P220] ORIG 0x0860dbb0 段：`call _Rb_tree_iterator::operator!=;
    // test %al,%al; je 0860dc0e(insert 分支)` —— 命中分支（+=）直落、insert 后置
    // ⇒ 原码是**正极性** `it != end()`；旧 `== end()` 负极性使 p167 出现
    // MISSING operator!= / EXTRA operator== 且两臂块序相反（MISSING/EXTRA 各 1）。
    if (it != m_data.end())
    {
        // [M4FreeNext15C.M4S1 round5 归位] ORIG 0x0860dbcd 段：`call ptEv` 后
        // `add $0x4,%eax / mov %eax,-0x1c(%ebp)`（具名 `&it->second` 物化槽），
        // 随后读/写各**重载一次**该槽（`mov -0x1c(%ebp),%ebx` / `%ecx`）并各自
        // 重算一遍索引 —— 即原码把 second 取址存具名指针局部（P329 反向：
        // ORIG 有具名物化、我方此前直接 `it->second.m_values[...]` 用 +4 disp 折平）。
        stCirculationStatistic* pStat = &it->second;
        pStat->m_values[user->get_charac_level()][field] += value;
    }
    else
    {
        stCirculationStatistic stat;
        memset(&stat, 0, 0x3540);
        stat.m_values[user->get_charac_level()][field] = value;
        m_data.insert(std::make_pair(user->GetServerGroup(), stat));
    }
    return true;
}