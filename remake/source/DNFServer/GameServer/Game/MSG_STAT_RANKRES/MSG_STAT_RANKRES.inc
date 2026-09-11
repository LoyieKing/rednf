// 值成员 m_result 由编译器默认构造（ORIG 仅 call GameResultType::C1，无
// placement new/异常路径）；析构同理（call GameResultType::D1）。
MSG_STAT_RANKRES::MSG_STAT_RANKRES()
{
}