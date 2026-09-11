// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081e8768 orig/ours=49/58 ndiff-blocks=8
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: mov    %eax,-0x10(%ebp) | mov    -0x10(%ebp),%eax | mov    &_ZN10GlobalData15s_event_managerE,%eax
void Dispatcher_GrowthCreatureChangeInfinityCreature::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081e8768：p4==0 → Ok(0x200)+事件(0x67) IsEventing(+0x34)→SendGrowthCreatureEventInfo；否则 Error
    stGrowthCreature& p = static_cast<stGrowthCreature&>(param);
    if (p.m_4 == 0)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x200);
        // ORIG 0x081e87a1..0x081e87bc：GetRepeatEvent 结果**不回存局部**（%eax 直用）：
        //   `mov (%eax),%edx; add $0x34,%edx; mov (%edx),%edx; … call *%edx`
        //   ⇒ 原码是链式单表达式 `…GetRepeatEvent(0x67)->IsEventing(0)`；
        //   旧码 `ev != 0 && ev->IsEventing(...)` 是**发明防御**（ORIG 不判空，语义轴）。
        if (GlobalData::s_event_manager->GetRepeatEvent(0x67)->IsEventing((CUser*)0))
        {
            // [P4 负结论 2026-09-11 实测回滚] 删 `ev2 != 0` 判空（改链式单表达式）实测
            //   ours 48→44 条 ⇒ gap +1→+5 **劣化**（ORIG 49 条侧等价物不止这一处判空）
            //   ⇒ 保留具名 ev2 + 判空形。
            CEventBase* ev2 = GlobalData::s_event_manager->GetRepeatEvent(0x67);
            if (ev2 != 0)
            {
                static_cast<GiveGrowCreatureEvent*>(ev2)->SendGrowthCreatureEventInfo(*user);
            }
        }
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x200,
                                 (unsigned char)(p.m_4 & 0xff));
    }
}