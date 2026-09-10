void HeroMissionEvent::sendMissionData(CUser* user)
{
    // [b28c-L7] 判词：ORIG 0x0816770e 段为 `mov (%eax);add $0x34;mov (%eax);
    // call *%edx`（vptr+0x34 虚调 IsEventing(0)）⇒ 必须虚调用形；直读
    // m_isEventing 产 movzbl 0x8(%eax) 伪差。
    if (!IsEventing(0))
        return;


    if (user == 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__, 0x307,
            "[HeroMission] Invalid user pointer. (processMission)");
        return;
    }

    unsigned int characNo = (unsigned int)user->get_charac_no(-1);
    std::map<unsigned int, std::vector<HeroMissionValue> >::iterator it =
        m_values.find(characNo);
    if (it == m_values.end())
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__, 0x30e,
            "[HeroMission] Not exist hero mission value. (charac:%u)",
            (unsigned int)user->get_charac_no(-1));
        return;
    }

    // 包 0x22f：short count × {mission(+0), +4, +8, +0xa}
    PacketGuard packet;
    packet.put_header(0, 0x22f);
    packet.put_short((int)it->second.size());
    for (std::vector<HeroMissionValue>::iterator vit = it->second.begin();
         vit != it->second.end(); ++vit)
    {
        packet.put_short(vit->m_mission);   // +0x00
        packet.put_short(vit->m_field4);    // +0x04
        packet.put_short(vit->m_field8);    // +0x08
        packet.put_short(vit->m_fieldA);    // +0x0a
    }
    packet.finalize(true);
    user->Send(packet);
