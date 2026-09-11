// 0x0868aaea
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868aaea | _ZN5CUser24SendOneADayItemShopIndexERKSt6vectorISt4pairIiiESaIS2_EE
void CUser::SendOneADayItemShopIndex(
    const std::vector<std::pair<int, int> >& list)
{
    // b28d-GrpD 按 ORIG 0x0868aaea 直证改写：
    //  ① ORIG +0x18 call GetRepeatEvent 之后**无判空**，+0x1d 直接 `mov (%eax),%edx`
    //     解引用虚表（判据①：删我方发明的 `event == 0` 防御）。
    //  ② ORIG +0x24 `mov %eax,(%esp)` 复用 GetRepeatEvent 的 %eax 作 this 调虚槽
    //     +0x34 ⇒ 源码是链式调用，**无 event 具名槽**；PacketGuard 落在 -0x1c，
    //     count -0x10，i -0xc，且每次 put_* 均 `lea -0x1c(%ebp),%eax` 重算
    //     ⇒ 删我方 -0x14 的 pkt 具名引用（P183/P329）。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x6d)->IsEventing(0))
        return;

    PacketGuard packet;                                          // ORIG -0x1c
    ((InterfacePacketBuf&)packet).put_header(0, 0x155);
    int count = (int)list.size();
    ((InterfacePacketBuf&)packet).put_byte(count);
    for (int i = 0; i < count; ++i)
    {
        ((InterfacePacketBuf&)packet).put_int(list[i].first);
        ((InterfacePacketBuf&)packet).put_int(list[i].second);
    }
    ((InterfacePacketBuf&)packet).finalize(true);
    Send(packet);
}