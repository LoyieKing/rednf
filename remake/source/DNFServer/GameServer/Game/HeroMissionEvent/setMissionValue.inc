// ---------------------------------------------------------------------------
// setMissionValue —— ORIG T 0x08167216
// _ZN16HeroMissionEvent15setMissionValueEP5CUserN20HeroMissionCondition9MissionNo1TEj
// （LinkE2.JpnExImpl 批次：objdump 0x8167216..0x81672bc 逐指令转写）。
// 判活：ORIG vptr+0x34 = CEventBase::IsEventing(NULL)，实现即读 +0x08 标志
//（本头布局镜像注释同证）。
// ---------------------------------------------------------------------------
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
void HeroMissionEvent::setMissionValue(CUser* user,
                                       HeroMissionCondition::MissionNo::T mission,
                                       unsigned int value)
{
    // ORIG 0x8167226：`mov (%eax);add $0x34;call *%edx` = 虚调用 IsEventing(NULL)。
    // 继承 CEventBase 后以真实虚调用复现（§4.5 正解），非直接读 m_isEventing。
    if (!IsEventing(0))
        return;

    if (user == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Event/EventHeroMission.cpp",
                              __PRETTY_FUNCTION__,
                              0x2a6, "[HeroMission] User is null.");
        return;
    }

    HeroMissionValue* out = 0;   // ORIG -0xc(%ebp) = 0
    if (findMissionValue(user, mission, out))
    {
        // ORIG mov %dx,0x8(%eax)：HeroMissionValue +0x08 写 ushort value
        out->m_field8 = (unsigned short)value;
    }
    sendMissionData(user);
}