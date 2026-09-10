int Dispatcher_GetTodayReward::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081251da：state>2 且 curCharacR 非空才受理（否则 0x287）；
    // get_binary 5 字节 → GetTodayRewardAndSendPostal(reward=int@+1)
    // 同 ClearAction：三条件直分支 + %al 常量岛 ⇒ 条件表达式直用（P226）。
    if (!(user != 0 && user->get_state() > 2 && user->getCurCharacR() != 0))
        return 0x287;
    // ORIG 0x0812520x：buf@-0xd，reward 取值走 `mov -0xc(%ebp),%eax`（+1 处 4 字节
    // 按值传）——用 packed 视图把「buf+1 解引用」变成字段访问，消 lea+add+mov 三步。
    struct __attribute__((packed)) RewardView {
        unsigned char m_head;    // +0x00
        _SIG_LOAD_ACTION_POINT::_TodayRewardItem m_reward;   // +0x01
    };
    RewardView buf;
    if (!packet.get_binary(reinterpret_cast<char*>(&buf), 5))
        return 0x28b;
    CUserProc::GetTodayRewardAndSendPostal(
        user, buf.m_reward);
    return 0;
