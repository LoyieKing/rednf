void CHeroMissionCommand::execute()
{
    CUser* user = GetUser();
    // ORIG P48：O0 无 CSE，三个调用表达式各含一次 Singleton::Get()（共 3 call），
    // 禁函数顶缓存 mission。set 分支先按右→左读 m_param(+0x10)/m_missionNo(+0xc)
    // 入 callee-saved 跨 Get() 保活（P87 形），再 call Get 作 receiver。
    if (m_str0 == "set")
    {
        // b28d-solo-GmCmds_D：ORIG 两处 `mov 0x10/0xc(%eax),%eax; mov %eax,%esi/%ebx`
        // 两段装载（4.1 对 enum 字段保留 eax→callee-saved 中转）⇒ 经 enum 字段
        // 视图 + 裸 register 局部复现（P254/P231；探针 /tmp/b28d_gmd/p6.cpp g10 全等）。
        register unsigned int param = static_cast<unsigned int>(
            ((const GmDHeroMissionCmdView*)this)->param);
        register unsigned int no = static_cast<unsigned int>(
            ((const GmDHeroMissionCmdView*)this)->no);
        ARAD::Singleton<HeroMissionEvent>::Get()->processMission(
            user, static_cast<HeroMissionCondition::MissionNo::T>(no), param);
    }
    else if (m_str0 == "reset")
    {
        ARAD::Singleton<HeroMissionEvent>::Get()->setEventData(user, "");
    }
    ARAD::Singleton<HeroMissionEvent>::Get()->sendMissionData(user);
