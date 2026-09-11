void CUser::setDungeonClearHackCheck(bool flag)
{
    if (flag) {
        // b28d-solo-CUser AE: ORIG 0x8649bab-0x8649bb4 `movzbl 0x8e93c; mov %eax,%edx;
        //   or $0x1,%edx`（无 cur 槽，sub $0x4）；具名 char cur 落 -0x2 多两段 ⇒ 内联。
        m_mcap.m_field54c = (m_mcap.m_field54c | 1);
    } else {
        m_mcap.m_field54c = (m_mcap.m_field54c | 2);   // ORIG 0x8649bc5-0x8649bce or $0x2,%edx 同形
    }
}