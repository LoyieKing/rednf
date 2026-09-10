void CAradRyosikaEvent::StartEvent()
{
    // P37 链式临时：实参提前物化 ebx/esi 跨 cMyTrace 构造存活，与 ORIG 同形。
    cMyTrace(__FUNCTION__, 0x14, 0)(
        "Arad Ryosika Event Start! [step:%d][job:%d]", m_field0c, m_field10);
    SetEventFlag(true);
