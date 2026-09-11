// C1 @ 0x080c9984：CEventBase() + vptr + new CBingo(0x1c) 包 shared_ptr<CBingo>
// 挂到 this+0xc。初始化列表 m_bingo(new CBingo()) 顺序与 ORIG 一致。
BingoEvent::BingoEvent()
    : m_bingo(new CBingo())
{
}