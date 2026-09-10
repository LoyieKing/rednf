void StackableAction::Action::init_function_type()
{
    // ORIG 0x0827f7c8：以 boost::bind 将 proc_* 成员注册进 map2（key 0x34/0x36/0x37）。
    // 与 ORIG 一致用 insert 方式（非 operator[]）。
    // [b28d-solo-GlobalData] p167 EXTRA `boost::arg<1>::C1Ev`×3：本头集下
    // `boost::_1` 不可用（COMPILE FAILED），保留 `arg<1>()` 构造形 ⇒ N（绑定库
    // placeholder 构造跨版本差异）封顶。
    using boost::arg;
    m_map2.insert(std::make_pair(TYPE_BINGO,
        boost::bind(&Action::proc_bingo, this, arg<1>())));
    m_map2.insert(std::make_pair(TYPE_LOTTERY_COUNT,
        boost::bind(&Action::proc_lotteryCount, this, arg<1>())));
    m_map2.insert(std::make_pair(TYPE_ADD_FATIGUE,
        boost::bind(&Action::proc_addFatigue, this, arg<1>())));
