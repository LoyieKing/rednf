void CBingoGmTest::reqValue()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }

    BingoEvent* event = static_cast<BingoEvent*>(
        static_cast<void*>(GlobalData::s_event_manager->GetRepeatEvent(0x6a)));
    if (event == 0)
    {
        return;
    }

    // [b23S3C] ORIG +0x48 单次 getBingoData→movzbl+cmp$2,setg 寄存器直测（无
    // lines 具名槽）；+0x74 每次 calNewValue 第 1 实参 = 独立 getBingoData() 的
    // bd 指针直传 uint&（ORIG mov %eax,%ebx；无 board 拷贝槽 -0x30）；
    // receiver 链 shared_ptr::operator-> 真调用（+0x8f）。
    if (static_cast<signed char>(
            static_cast<BingoData*>(user->getBingoData())->m_pad4) > 2)
    {
        return;
    }

    int oldBoard = static_cast<int>(
        static_cast<BingoData*>(user->getBingoData())->m_eventId);
    int newValue = (*reinterpret_cast<boost::shared_ptr<CBingo>*>(
                        event->getBingoSystem()))
                       ->calNewValue(
                           static_cast<BingoData*>(user->getBingoData())
                               ->m_eventId,
                           m_subCmd);

    if (*reinterpret_cast<int*>(user->getBingoData()) != oldBoard)
    {
        static_cast<BingoData*>(user->getBingoData())->m_needSave = true;
        int gained = newValue - static_cast<int>(
            static_cast<BingoData*>(user->getBingoData())->m_pad4);
        if (gained > 0)
        {
            // [b23S3C] ORIG +0x0f5 分支形 clamp（读槽→cmp reg→jle→mov $3→写回）。
            // [b29-Lane21] 实测：if 形残差 4 行（4.4 折 `cmpl $3,-0x10; movl $3,-0x10`）；
            //   三目形 `(gained>3)?3:gained` 残差 5 行（4.4 发 `cmovle -0x10,%eax`，本 TU
            //   无 -march=i486 ⇒ cmov 族不可消，P230①/Main 路由域）⇒ 保留 if 形。
            //   残差 = N18b/P321-T5（禁为装载序改类型）：ORIG `mov -0x10,%eax; cmp $3,%eax;
            //   jle; mov $3,%eax; mov %eax,-0x10`（寄存器比较 + 常量经 eax 统一写回）。
            if (gained > 3)
                gained = 3;
            static_cast<BingoData*>(user->getBingoData())->m_pad4 =
                static_cast<char>(newValue);
            for (int i = 0; i < gained; ++i)
            {
                event->sendOneMatchLineReward(*user);
            }
        }
    }

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x1c3);
    packet.put_byte(m_subCmd);
    packet.put_int(*reinterpret_cast<int*>(user->getBingoData()));
    packet.finalize(true);
    user->Send(packet);
