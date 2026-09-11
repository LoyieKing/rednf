// D1 @ 0x080c9a06：vptr + shared_ptr<CBingo> 析构 + CEventBase 析构（编译器按
// 声明逆序生成：m_bingo 先于 CEventBase）。D0 @ 0x080c9a68 由编译器生成。
BingoEvent::~BingoEvent()
{
}