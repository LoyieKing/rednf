// ============================================================================
// 2. Dispatcher_Avatar_Disjoint_Random_Reward::send  ORIG 0x081dec56
//    拷贝 stAvatarDisjointRandomReward（含 vector 拷贝），按结果码分支发包。
// [b28d-solo-DispLinkA1] verdict=AE（compare_tu clean；p167 25/25 CLEAR）。
//   修法：P96 迭代器改非 const iterator；P13+ORIG 多一次 size() 并以非零包裹
//   循环（setne 物化）；P28 it++（ORIG operator++(int)）。
// ============================================================================
void Dispatcher_Avatar_Disjoint_Random_Reward::send(CUser* user, ParamBase& param)
{
    // ORIG C1 @0x0823516e 为拷贝构造；ParamBase 实际类型即本 Param。
    stAvatarDisjointRandomReward result(
        static_cast<stAvatarDisjointRandomReward&>(param));

    PacketGuard packet;
    packet.put_header(1, 0x1c2);
    if (result.m_4 == 0) {
        packet.put_byte(1);
        packet.put_short(result.m_8);
        packet.put_byte((int)result.m_rewards.size());
        // [b28d-solo-DispLinkA1 P96/P13] ORIG 迭代器为非 const iterator
        // （p167: operator-><pair<int,int>*> / ne<pair*,pair*>），且 size() 被
        // 调用两次：一次作 put_byte 实参、一次以其非零包裹循环（setne 物化）。
        if (result.m_rewards.size() != 0) {
            for (std::vector<std::pair<int, int> >::iterator it =
                     result.m_rewards.begin();
                 it != result.m_rewards.end(); ++it) {
                packet.put_int(it->first);
                packet.put_int(it->second);
            }
        }
    }
    else {
        packet.put_byte(0);
        packet.put_byte(result.m_4);
    }
    packet.finalize(true);
    user->Send(packet);
}