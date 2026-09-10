void Dispatcher_Client_Spec_Statistic::send(CUser* user, ParamBase& param)
{
    if (user == 0)
    {
        return;
    }
    // [G1Frest 2026-09-10] ORIG 0x081ce7a9 `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)`
    //   ⇒ param 具名指针局部 -0x10（P0 槽往返，非内联 &param）。
    ParamBase* p = &param;
    char* hwSpec = user->getClientHWSpec();                        // -0xc
    // ORIG 0x081ce7c0/0x081ce7d2：p[4] 两次独立重算（P48/P0）
    //   ⇒ 禁 specType/special 具名缓存（旧码 -0x21/-0x20 两槽为发明）。
    if (hwSpec[0] != 0 &&
        *(unsigned char*)((char*)p + 4) != 1 &&
        *(unsigned char*)((char*)p + 4) != 0x81)
    {
        return;
    }
    // ORIG 0x081ce7e1 `lea -0xc16(%ebp),%eax; mov %eax,(%esp); call C1`
    //   ⇒ 栈对象直接构造。旧码 `new (stat) Packet_Client_Spec_Statistic()` 在 4.4 展开
    //   _ZnwjPv/_ZdlPvS_/_Unwind_Resume + push %edi + 帧 +0x1c（N83 我方侧形），
    //   而 ORIG 只有一条 call（4.1 前端内联 placement new）。
    Packet_Client_Spec_Statistic stat;
    if (*(unsigned char*)((char*)p + 4) == 1 ||
        *(unsigned char*)((char*)p + 4) == 0x81)
    {
        stat.m_type = 2;                                           // ORIG 0x081ce805 `movb $0x2,-0xc0c`
    }
    else
    {
        stat.m_type = 0;                                           // ORIG 0x081ce80e `movb $0x0,-0xc0c`
    }
    // ORIG 0x081ce81c `mov %al,-0xc0b` = 写成员 stat+0xb（非独立局部）
    stat.m_count = (unsigned char)hwSpec[1];
    memcpy(&stat.m_specs[0], hwSpec + 4, (unsigned int)stat.m_count * 0xc);
    // ORIG 0x081ce86a `mov %ax,-0xc14` = 写成员 stat.packetSize(+0x2)
    stat.packetSize = (unsigned short)((unsigned int)stat.m_count * 0xc + 0x12);
    // ORIG 0x081ce89e 区：GetServerGroup → GetServerProxy → SendPacket，
    //   无 proxy 判空（旧码 `if (proxy != 0)` 为发明防御）；实参 &stat/packetSize
    //   先求值并经 esi/ebx 跨调用保活 ⇒ 链式调用形。
    GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&stat), (int)stat.packetSize);
