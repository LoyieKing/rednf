// ---- 查询 ----
bool CCargo::check_slot_empty(int idx) const
{
    char framePad[0x10];
    (void)framePad;
    if (!IsOperatorable(idx))
        return 0;
    // [b28f-SweepGroup17] ORIG 0x0850bbca 尾为分支岛形（`test; jne <skip>;
    //   mov $0x1,%eax; jmp <end>; skip: mov $0x0,%eax`），三目/直表达式在 4.4
    //   恒折 `sete %al`（P47/P137 块排布）⇒ 改 if/return 两段式。
    if (m_pItems[idx].m_addInfo == 0)
        return 1;
    return 0;
}