void CNPCDynamicInfoManager::onTimer()
{
    // 声明序经实验对齐 ORIG 栈槽（count -0x12 / mood -0x10 / curMood -0xc /
    // guard -0x20 / idx -0x24 / it -0x28 / endIt -0x2c）。
    short count;
    ENUM_NPC_MOOD mood;
    ENUM_NPC_MOOD curMood;
    PacketGuard guard;
    int idx;

    guard.put_header(0, 0xc3);
    idx = guard.get_index();
    count = 0;
    guard.put_short(0);
    // 块内声明 + 声明即初始化：begin/end 直出 it/endIt 槽（无临时中转），
    // 且执行点落在 put_short(0) 之后，与 ORIG +0x055/+0x067 位置一致。
    std::map<unsigned int, CNPCDynamicInfo*>::iterator it = m_map.begin();
    std::map<unsigned int, CNPCDynamicInfo*>::iterator endIt = m_map.end();
    while (it != endIt) {
        mood = it->second->getMood();
        curMood = it->second->changeMood(false);
        if (mood != curMood) {
            it->second->makeNotiPacketCurMood(guard);
            ++count;
        }
        ++it;
    }
    guard.put_short(idx, count);
    guard.finalize(true);
    if (count != 0)
        G_GameWorld()->send_all(guard);
