// RandomOptionSeed::reset（ORIG W 0x080cb2cc，m_field=0）：定义原在
// Inven_Item.cpp，迁入本 TU（RandomOption::reset 的 callee 之一）——
// RandomOption::reset 所在 TU 不再含全部 callee 定义体 ⇒ 4.4 unit-at-a-time
// 不塌缩 outgoing 帧，保持 ORIG sub $0x18（NL4/NL3 unit-at-a-time 实证）。
void RandomOptionSeed::reset()
{
    m_value = 0;
}