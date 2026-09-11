// b28d-Main：`Party::~Party()` 已改为 Party.h 内 inline 定义（ORIG 该符号在头内可见，
// 否则 map<int,Party>::construct 会多发 16 条 EH landing pad）；此处不得再定义（重定义）。
void Party::Reset()
{
    PlayInfo::Reset();
    m_f3c = 0;
}