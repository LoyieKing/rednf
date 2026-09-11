// OnDestroyRoom ORIG T 0x8562dc6 报告 OnDestroyRoom.md
// [b28 BM2 判词·N7] AE-ndiff=6（fresh .o 归一）：pWaitingRoomList_->DelRoom 虚槽
// 调用 ORIG 4.1 以 %ecx 承载 fnptr、%edx/%eax 装载实参，4.4 恒 %eax 承载 call 目标
// （N7「间接 call 目标恒 %eax」，b26 NL28 族）。助记符/条数全等，纯寄存器文本差；
// 禁绑定形（P0/N96）。
void CFairMatch::OnDestroyRoom(CUser* /*user*/)
{
    m_field04 = 0;
    pWaitingRoomList_->DelRoom(this);        // 等待表 vtable[+0x04]=DelRoom(this)
}