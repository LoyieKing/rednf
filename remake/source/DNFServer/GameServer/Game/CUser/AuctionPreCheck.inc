// ORIG T 0x08680a96：拍卖操作前置门（AuctionPreCheck.md）。
// 控制流：
//   1) pay==1（cera 拍卖）且 CCeraAuctionServerProxy::IsRunning()!=1 → 报 0x8c，返回 0；
//   2) pay==0（普通拍卖）且 CAuctionServerProxy::IsRunning()!=1 → 报 0x8c，返回 0；
//   3) get_state()<3 或 getCurCharacR()==0 → 返回 0x8151；
//   4) CSecu_ProtectionField::Check(this, 0x35) !=0 → 报对应错误，返回 0；
//   5) 全部通过 → 返回 -1。
// b28d-lane5 N封顶: N96+P168a④ 清 `register bool fail asm("al")` 后 bool 暂存落栈槽
// （movb $1,-0xd / cmpb $0,-0xd），ORIG 为寄存器 `mov $1,%eax`；无绑定即不可达（P0 硬约束）。
// | ORIG 0x08680a96 | _ZN5CUser15AuctionPreCheckE14ENUM_CMDPACKET8PAY_TYPE
int CUser::AuctionPreCheck(ENUM_CMDPACKET cmd, PAY_TYPE pay)
{
    // b28d-solo-CUser 重写（2026-09-09，P226/P214①）：ORIG 无 fail 具名局部——两段门
    //   均为「pay 值比较 && !IsRunning()」**直接 if 短路**（4.1 物化 `mov $1,%eax;
    //   jmp; mov $0,%eax; test %al,%al; je` 到寄存器，0 槽往返）；旧 `bool fail`+goto
    //   SET01/SET02 结构被 4.4 落槽（movb $1,-0xd / cmpb $0,-0xd）。裸 if 形即复现。
    if (pay == (PAY_TYPE)1 && !GlobalData::s_cera_auction_proxy->IsRunning())
    {
        SendCmdErrorPacket(cmd, 0x8c, pay);
        return 0;
    }

    if (pay == (PAY_TYPE)0 && !GlobalData::s_auction_proxy->IsRunning())
    {
        SendCmdErrorPacket(cmd, 0x8c, pay);
        return 0;
    }

    if (get_state() <= 2 || getCurCharacR() == 0)
    {
        return 0x8151;
    }

    int err =
        GlobalData::s_pSecuProtectionField->Check(this, (SECURITY_PROTCTION)0x35);
    if (err != 0)
    {
        SendCmdErrorPacket(cmd, (unsigned char)err, pay);
        return 0;
    }

    return -1;
}