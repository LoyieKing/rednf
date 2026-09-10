bool Inter_LoadEtc::SendResult(CUser* user)
{
    // [Lane17 I 判词] ORIG 0x084eb2a7：p167 CLEAR（72/72）。判据⑪ 直方图显示
    // **相对序差**（ORIG −0x44 ↔ 我方 −0x3c 差 8、ORIG −0x28 ↔ 我方 −0x2c 差 4、
    // −0x50↔−0x48、−0x54↔−0x4c、−0x24↔−0x28 交错）⇒ (a) 类、判据⑩ 可攻；
    // 本轮未做（预算）⇒ 记「待攻」。旧判词：DIFF 402/397。
    // 本轮已修：`for (unsigned j…)` → `int j`（P2：ORIG 尾测 setle，旧 setbe 为 unsigned）。
    // 残差 71 块：packet/it 槽组 -0x3c vs -0x44（N98 首次使用序）+ `m_type==0x21`
    // 的 setcc-fold（ORIG `cmp $0x21; sete; test; jne` vs 4.4 直跳 jne，跨版本 N）。
    PacketGuard packet;
    packet.put_header(1, 4);
    packet.put_byte(1);
    packet.put_int(user->getRegTime());
    packet.put_int((int)user->getCurCharacCreateTime());
    packet.put_short(user->get_unique_id() & 0xffff);
    packet.put_short(user->getCurCharacTotalFatigue());
    packet.put_short(user->getCurCharacTotalMaxFatigue());
    packet.put_short(user->getCurCharacUsedFatigue());

    WongWork::CUserPremium* premium = user->GetPremiumInfo();
    // ORIG 无 null 检查，直接解引用调用 GetPremiumInfoList(0x270f)。
    // [b28d-solo-Inter_LoadEtc_impl] P96/P270：ORIG 调 begin() const/end() const
    // ⇒ 容器为 const 局部（非 const 版会多转换构造 + ne(PKi,Pi) 混合实例）。
    const std::vector<WongWork::SUserPremiumInfo> infos =
        premium->GetPremiumInfoList(0x270f);
    int index = packet.get_index();
    int count = 0;
    packet.put_byte(0);
    long now = GlobalData::s_systemTime_.getCurSec();
    // [b28d-solo-Inter_LoadEtc_impl] P167 判据：ORIG 用 const_iterator + `it->`
    // （operator-> ×4 / operator* ×1 / operator++ ×1），且 PvP 检查只有一处
    // （我方旧 `[]` 循环 + 重复 if 多发 G_GameWorld/IsPvPSkilTreeChannel/get_pvp_grade/size/op[]）。
    for (std::vector<WongWork::SUserPremiumInfo>::const_iterator it = infos.begin();
         it != infos.end(); ++it)
    {
        // [b28d-solo-Inter_LoadEtc_impl] ORIG 0x084c2cfc `call operator*` + 5 dword
        // 块拷贝到 -0x74（值拷贝具名局部，P0 槽往返证据）；字段读仍走 `it->`（ptEv）。
        WongWork::SUserPremiumInfo info = *it;
        (void)info;
        if (it->m_state == 2)                        // ENUM_PREMIUM_STATE_2 等待开始
            continue;
        if (G_GameWorld()->IsPvPSkilTreeChannel() && user->get_pvp_grade() <= 9 &&
            it->m_type == 0x21)
            continue;
        packet.put_byte(it->m_type & 0xff);
        packet.put_int(it->m_end - (int)now);
        ++count;
    }

    // [b28d-solo-Inter_LoadEtc_impl] P167 补缺口：ORIG 0x084c2e0a `put_byte(int&,int)`
    // （index 计数回填）+ 0x084c2e15 `CUser::GetCera()` + 0x084c2e24 `put_int`。
    packet.put_byte(index, count);
    packet.put_int(user->GetCera());

    if (user->getCurCharacR() == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1323, 5);
        trace("bool Inter_LoadEtc::SendResult()");
        return false;
    }

    user->verifyDoingQuest();
    // [b28d-solo-Inter_LoadEtc_impl] P48/P167：ORIG 每处重取 getCurCharacQuestR
    // （0x084c2e9a/0x084c2ed0/0x084c2ef7/0x084c2f3a 四调用点），无 questR 缓存局部。
    for (int i = 0; i < 0x14; ++i)
    {
        if (user->getCurCharacQuestR()->m_quest[i] == 0)
            packet.put_short(0xffff);
        else
            packet.put_short(user->getCurCharacQuestR()->m_quest[i] & 0xffff);
        packet.put_int(user->getCurCharacQuestR()->m_trigger[i]);
    }
    for (int i = 0; i < 5; ++i)
    {
        int v = *reinterpret_cast<const int*>(
            &user->getCurCharacQuestR()->m_authenData.m_authen[i * 4]);
        packet.put_int(v);
    }
    packet.put_byte((int)(signed char)user->getCurCharacVill());
    packet.put_byte(user->GetTutorialSkipable());

    int tIndex = packet.get_index();
    int tCount = 0;
    packet.put_byte(0);
    // [b28d-solo-Inter_LoadEtc_impl] P2：ORIG 尾测 `setle`（有符号）
    // ⇒ 循环变量为 int（`unsigned` 生 `setbe`）。
    for (int j = 0; j < 0x80; ++j)
    {
        if (user->getCurCharacEscaladeTutorialFlag(j) != 0)
        {
            packet.put_byte((int)j);
            ++tCount;
        }
    }
    packet.put_byte(tIndex, tCount);
    packet.put_short(user->GetFatigueBatteryCharging());
    packet.put_short(user->getCurCharacFatigueGrownUpBuff());

    char punishOverThirty = user->isTradePunishOverThirtyDays() ? 1 : 0;
    packet.put_byte(punishOverThirty);
    packet.finalize(true);
    user->Send(packet);
    AvatarCoin::SendSyncPacket(user);
    return true;
