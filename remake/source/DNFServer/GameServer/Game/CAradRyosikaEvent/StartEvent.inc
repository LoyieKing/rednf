// ORIG 0x081a0f1e：cMyTrace(__FUNCTION__, 0x14, 0)("Arad Ryosika Event Start!
//   [step:%d][job:%d]", m_field0c, m_field10) 在 SetEventFlag(true) 之前
//   （trace 先、flag 后，ORIG 汇编序）。__FUNCTION__ 名（0x0b 长度符号：
//   ORIG r _ZZN17CAradRyosikaEvent10StartEventEvE12__FUNCTION__ @0x08bab950
//   sz 0x0b，"StartEvent\0"）。必须传 __FUNCTION__ 本体（P139），手写字面量
//   只生成匿名 .rodata，丢 ORIG 局部静态符号。
void CAradRyosikaEvent::StartEvent()
{
    // P37 链式临时：实参提前物化 ebx/esi 跨 cMyTrace 构造存活，与 ORIG 同形。
    cMyTrace(__FUNCTION__, 0x14, 0)(
        "Arad Ryosika Event Start! [step:%d][job:%d]", m_field0c, m_field10);
    SetEventFlag(true);
}