// GI 移位（SURPLUS cand1，ORIG GI 地址序最小）：本函数原位于 5334 行区，纯移位
// 上送至本 TU 首个强实体 tagPacketHeader::getSize 之前，使 _GLOBAL__I_ 后缀对齐
// ORIG _GLOBAL__I__ZN13CBusinessImpl18SendBuffertoClientEiiijPhs。语义零变化。
int CBusinessImpl::SendBuffertoClient(int a, int b, int c, unsigned int d,
                                      unsigned char* data, short len)
{
    (void)b;
    (void)c;
    // ORIG 0x080de240：按 stream 长档位取池；<<0x252 <<-1；
    // put_binary({int a, uint d, short len}, 0xc)；put_binary(data, len)。
    const int size = (int)len + 0x14;
    CStreamGuard guard(true);
    // ORIG 0x080de26c..0x080de2e3 三分支形态：首支 `cmpl $0xfff,-0xc; jg else`
    // （= size <= 0xfff）；次支重测 `cmpl $0xfff; jle big` + `cmpl $0x3fff; jg big`
    // ⇒ 源码在 else-if 中显式重复 `size > 0xfff`，并非纯区间判断。
    if (size <= 0xfff)
    {
        guard.Set(GlobalData::s_small_stream_pool->Acquire("BusinessImpl.cpp", 0x3a));
    }
    else if (size > 0xfff && size <= 0x3fff)
    {
        guard.Set(GlobalData::s_stream_pool->Acquire("BusinessImpl.cpp", 0x3e));
    }
    else
    {
        guard.Set(GlobalData::s_big_stream_pool->Acquire("BusinessImpl.cpp", 0x42));
    }
    **guard << 0x252;
    **guard << -1;
    struct
    {
        int field00;
        unsigned int field04;
        unsigned short field08;   // ORIG 0x080de355 `mov %ax,-0x18(%ebp)`（2 字节存）
    } hdr;
    hdr.field00 = a;
    hdr.field04 = d;
    hdr.field08 = (unsigned short)(len);   /* movzwl %ax */
    guard->put_binary(&hdr, 0xc);
    guard->put_binary(data, (int)len);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    // ORIG 0x080de3b9：`return 0` 先物化到 %ebx，再析构 guard，最后 mov %ebx,%eax。
    return 0;
}