void StackableAction::Action::init_function_type()
{
    // ORIG 0x0827f7c8：以 boost::bind 将 proc_* 成员注册进 map2（key 0x34/0x36/0x37）。
    // 与 ORIG 一致用 insert 方式（非 operator[]）。
    // [b28d-solo-GlobalData] p167 EXTRA `boost::arg<1>::C1Ev`×3：本头集下
    // `boost::_1` 不可用（COMPILE FAILED），保留 `arg<1>()` 构造形 ⇒ N（绑定库
    // placeholder 构造跨版本差异）封顶。
    // [G7-sub] 本轮复核：ORIG 157 / 我方 169，gap 12 恰为 3×`call
    //   _ZN5boost3argILi1EEC1Ev`（p167 EXTRA，每处 lea+mov+call+sub=4 条）。
    //   ORIG 侧占位符是**未初始化字节槽直传**、无 call：
    //   0x0827f893 `movzbl -0xe9(%ebp),%eax; mov %al,0x10(%esp)`、
    //   0x0827f95c `movzbl -0xd9(%ebp),%ebx; mov %bl,0x10(%esp)`。
    //   本头集 `boost::_1` 编译失败（前轮实证）⇒ 维持 N 封顶。
    using boost::arg;
    m_map2.insert(std::make_pair(TYPE_BINGO,
        boost::bind(&Action::proc_bingo, this, arg<1>())));
    m_map2.insert(std::make_pair(TYPE_LOTTERY_COUNT,
        boost::bind(&Action::proc_lotteryCount, this, arg<1>())));
    m_map2.insert(std::make_pair(TYPE_ADD_FATIGUE,
        boost::bind(&Action::proc_addFatigue, this, arg<1>())));
