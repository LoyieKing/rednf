// ORIG 0x081958b6：Guard<Mutex> 后 std::find（operator== 三字段比较），命中即 true。
// [b28d-28] DIFF | ORIG 0x081958b6 | N86/P28：std::find 迭代器槽序 + EH 块序。
// [L4fEventAvatar 2026-09-11] P0：ORIG 0x081958c8..0x08195936 的调用序为
//   end(-0x14) → begin(-0x10) → find(-0x1c) → end2(-0xc)，即 find 结果先落具名
//   迭代器槽（sret 直达、无额外拷贝槽）再与 end() 比较；直写 `find(...) != end()`
//   会让 4.4 先求值比较右侧 end2（-0x18）打乱槽序。
bool Arad_EventPeriodDataManager::isRewarded(EventRewardItemArad item)
// [P8S2 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
{
    Guard<Mutex> guard(&m_mutex);
    std::list<EventRewardItemArad>::iterator it =
        std::find(m_rewardItemList.begin(), m_rewardItemList.end(), item);
    // ORIG 0x08195950 `test %al,%al; je <mov $0,%ebx>` ⇒ 分支物化形
    // （4.1 对 bool 返回表达式 + 尾部 Guard 析构的产物），非 4.4 的 `mov %eax,%ebx`。
    // ORIG 0x0819593e `lea -0x1c; mov 0x4(%esp)` + `lea -0xc; mov (%esp)`
    //   ⇒ 实参位 (a=-0xc=end2, b=-0x1c=it)：源码写 `end() != it`。
    if (m_rewardItemList.end() != it)
        return true;
    return false;
}