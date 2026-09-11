// getBingoSystem @ 0x080c9fea：返回 this+0xc —— 成员 boost::shared_ptr<CBingo>
// 实例地址（ORIG ctor 将 new CBingo(0x1c) 挂到该槽；调用方经 operator-> 解引用）。
void* BingoEvent::getBingoSystem()
{
    return &m_bingo;   // +0x0c boost::shared_ptr<CBingo> 实例地址
}