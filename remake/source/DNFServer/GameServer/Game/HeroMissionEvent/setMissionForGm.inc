void HeroMissionEvent::setMissionForGm(CUser* user,
                                       HeroMissionCondition::MissionNo::T mission,
                                       unsigned int value)
{
    HeroMissionValue* out = 0;   // ORIG -0x10(%ebp) = 0
    if (findMissionValue(user, mission, out))
    {
        std::map<HeroMissionCondition::MissionNo::T,
                 BaseHeroMissionCondition*>::iterator it =
            m_conditions.find(mission);
        // [b28c-L7] P223：ORIG 0x08167a28 `test %al; jne <nop>` + body 尾
        // `jmp <leave>` + `+0x8a: nop` 空块占位岛 ⇒ 复现形 `if(it==end) goto END;
        // body; END:;`（4.4 对 `if(it!=end){body}` 恒 jump-thread 少 jmp/nop）。
        if (it == m_conditions.end())
            goto END;
        // ORIG mov %dx,0x4(%eax)：HeroMissionValue +0x04 写 ushort GM 值
        out->m_field4 = (unsigned short)value;
    }
END:;
