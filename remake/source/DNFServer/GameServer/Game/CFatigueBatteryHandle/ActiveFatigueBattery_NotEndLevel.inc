short CFatigueBatteryHandle::ActiveFatigueBattery_NotEndLevel(CUser* user)
{
    // b28d-20: ORIG 0x084a829e — consumed=0 最先（movl $0,-0x20），battery 次
    //（-0x1c），sp 引用最后（-0x18，add $0x68）；守卫形 = 嵌套 if + B 否定早返
    //（探针 /tmp/p_fat4.cpp neF 逐条复现 ORIG：A 假 `jg end` 直接落末尾 return，
    // B 真 `jne body`、B 假 `mov consumed; jmp end`、body 尾 fall-through 到 end）。
    // packet 构造/发送按 ORIG 次序，SendPacket 走
    // `s_statistic_proxy_mgr->GetServerProxy(0)->SendPacket(...)` 单语句链式
    //（P52：packet 地址入 %ebx 跨 GetServerProxy 保活）。
    int consumed = 0;
    int battery = static_cast<int>(static_cast<short>(user->GetFatigueBatteryCharging()));
    ServerParameterScript& sp = G_CDataManager()->m_serverParameter;

    if (static_cast<int>(sp.m_fatigueTable5c0.m_batteryThreshold1a8) <= battery)
    {
        if (static_cast<short>(sp.m_fatigueTable5c0.m_activeSwitch1a4) == 0)
            return static_cast<short>(consumed);

        // bonus = 倍率(u16)*battery / 开关(u16)；乘积取 32 位形态，除法有符号 idivl。
        // 倍率不做显式 (unsigned int) cast：ushort→int 自然提升令 4.4 产 ORIG 的
        // `movzwl 0x766(%eax),%eax; movzwl %ax,%eax; imul battery_slot,%eax` 形
        //（探针 /tmp/p_fat4.cpp mulD；加 cast 反产 `movzwl %ax,%edx` + reg-imul 3 条）。
        int bonus = static_cast<int>(
            sp.m_fatigueTable5c0.m_bonusMultiplier1a6 * battery) /
            static_cast<int>(sp.m_fatigueTable5c0.m_activeSwitch1a4);
        consumed = battery;

        int grownUpBuff =
            static_cast<int>(user->getCurCharacFatigueGrownUpBuff() & 0xffff) + bonus;
        unsigned short limit =
            sp.m_fatigueTable5c0.getLimitGrownupBuffCntFatigueBattery(
                user->get_charac_level());
        if (static_cast<int>(limit) < grownUpBuff)  // 有符号比较（jge）
            grownUpBuff = limit;

        Packet_Fatigue_Battery_Money_Statistic packet;
        packet.m_serverGroup = static_cast<unsigned char>(G_CEnvironment()->get_server_group());
        packet.m_grownupBuff = static_cast<unsigned short>(grownUpBuff);
        GlobalData::s_statistic_proxy_mgr->GetServerProxy(static_cast<ENUM_SERVER_GROUP>(0))
            ->SendPacket(reinterpret_cast<char*>(&packet), 0x11);
        user->setCurCharacFatigueGrownUpBuff(static_cast<unsigned short>(grownUpBuff));
    }
    return static_cast<short>(consumed);
