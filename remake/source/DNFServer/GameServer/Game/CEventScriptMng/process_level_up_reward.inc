// [b28d-solo-CEventScriptMng] process_level_up_reward：AE。
// ORIG 0x0810bf56：三条独立守卫（user==0 / !is_eventing / reward!=0 正向包裹，
// P133 + P214①；尾部两岛 nop;jmp + nop 与 ORIG 全等）；check_recv_level 结果
// 无具名局部（P0：call 结果寄存器直测）；flag 形参入口 spill 到 -0x2c（bool
// 形参一次性截断，P244）。
// 三个 callee 返回类型链：ORIG 0x0810bfb8/0x0810c005/0x0810c000/0x0810c07c 均
// `call; test %al,%al; je`；4.4.7 探针对 char/int 返回恒产 4 条（调用点
// cast/!=0/!! 四形全败），仅 bool 返回产 2 条 ⇒ 由 Main 代落
// CEventScriptMng.h is_recv_check_item/check_recv_level/set_reward_item → bool
// （定义端 EventLevelupA2/CEventScriptMng_entities 同步；P248 档①/P315）后
// 6 条残差归零。p167 CLEAR（16/16），语义闭合。
void CEventScriptMng::process_level_up_reward(CUser* user, bool flag)
{
    if (user == 0)
        return;
    if (!is_eventing(0x194))
        return;

    CLevelUpReward* reward =
        static_cast<CLevelUpReward*>(get_event_entity(0x194));
    if (reward != 0)
    {
        if (reward->is_recv_check_item() != 0)
        {
            if (reward->check_recv_level(user->get_charac_level(),
                                         user->get_charac_job(),
                                         user->getCurCharFirstGrowType(),
                                         flag) != 0)
                DB_SelectLevelUpReward::makeRequest(
                    static_cast<int>(user->GetUID()),
                    user->getCurCharacNo(),
                    user->get_charac_level());
        }
        else if (reward->set_reward_item(user->get_charac_level(),
                                         user->get_charac_job(),
                                         user->getCurCharFirstGrowType()) != 0)
        {
            reward->do_action(user);
        }
    }
}