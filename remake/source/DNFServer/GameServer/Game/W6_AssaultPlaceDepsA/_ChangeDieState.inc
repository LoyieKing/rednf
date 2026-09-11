// ---------------------------------------------------------------------------
// ORIG T 0x82ebaf8 (size 0xa3)：_ChangeDieState(int)
// ---------------------------------------------------------------------------
void CAssaultPlace::_ChangeDieState(int index)
{
    m_assaulters[index].SetState((ENUM_ASSAULT_STATE)6);
    // b28 BM4: P223 goto 尾空标签——ORIG +0x4a jne→+0xa0 nop 岛 + body 尾
    // +0x9e jmp +0xa1（eventing-true 落点在 body 之后 leave 之前）。嵌套
    // if{}else{} 形 4.4 jump-thread 合并落点缺此 jmp；goto 形保留 nop 中继。
    // 短路 `||` 段沿用 BL6 v6（!boolcallee 的 xor$1 直传形对齐 +0x59）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x20)->IsEventing((CUser*)0))
        goto EVENT_DONE;
    if (!GlobalData::s_power_manager->IsPowerWarEventOn()
        || ((CUserCharacInfo*)m_assaulters[index].GetUser())
               ->getCurCharacVill() != 7)
        m_assaulters[index].DieUser();
EVENT_DONE:
    ;
}