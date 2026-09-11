// ORIG T 0x0864d9a4 sz0x17b：header(0,0x44)；flag=true 单条，false 全列表。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864d9a4 | _ZN5CUser19SendRestrictedGoodsEbi
void CUser::SendRestrictedGoods(bool flag, int itemIdx)
{
    PacketGuard packet;
    // ORIG 0x0864d9b2..0x0864dad6：无 pkt 具名指针槽（-0xc 槽不存在），每次访问都
    // 重发 `lea -0x14(%ebp),%eax; mov %eax,(%esp)` ⇒ P183 不缓存 cast 结果。
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x44);
    // ORIG 0x0864d9d8 `movzbl -0x2c(%ebp),%eax; xor $0x1,%eax; test %al,%al;
    //   je <put_byte(0) 块>`，put_byte(1) 块 fall-through ⇒ 4.1 的 then/else 物理序相反：
    //   写 `if (flag) {A} else {B}` 4.4 产 `cmpb $0,-0x2c; je <B>`（我方 then fall-through）。
    //   反写 `if (!flag) {B} else {A}` 才逐条复现（P59 双向布局律），语义等价。
    if (!flag)
    {
        ((InterfacePacketBuf*)&packet)->put_byte(1);
        ((InterfacePacketBuf*)&packet)->put_short((int)m_list711ec.size());
        // b28d-solo-CUser p167：同 IsRestrictedGoods，ORIG 用非 const `_List_iterator<int>`。
        // ORIG 0x0864da22(选 it)/0x0864da3d(选 end) 把 end 在循环**前**缓存一次（P13）；
        //   `it != m_list711ec.end()` 会让 4.4 每轮回调 end()。
        for (std::list<int>::iterator it = m_list711ec.begin(),
                                      endIt = m_list711ec.end();
             it != endIt; ++it)
        {
            ((InterfacePacketBuf*)&packet)->put_int(*it);
        }
    }
    else
    {
        ((InterfacePacketBuf*)&packet)->put_byte(0);
        ((InterfacePacketBuf*)&packet)->put_short(1);
        ((InterfacePacketBuf*)&packet)->put_int(itemIdx);
    }
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
}