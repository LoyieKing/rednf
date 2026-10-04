// [b28d-lane2] verdict=DIFF ndiff=52（ORIG 0x084dd12e）：寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=45（ORIG 0x84dd12e）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_Set_CleanPad_Point::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // ORIG 084dd137/084dd13b：`cmpl; je +0xb3`（⇒ 单出口 `if (user != 0) { ... } return 0;`）。
    if (user != 0)
    {
        // ORIG 084dd141：pPkt 槽在 user 非空门之后物化（084dd144 mov %eax,-0x1c）。
        Inter_Set_CleanPad_PointPktView* pPkt = reinterpret_cast<Inter_Set_CleanPad_PointPktView*>(data);
        user->m_mcap.setPoint(pPkt->m_point);
        // ORIG 084dd171/084dd17b：getPoint 与 pPkt->m_point 无栈槽，分别以 %esi/%edi 跨
        // NumberToString 保活（P227 全表达式临时链）⇒ 实参直接内联，勿建具名局部。
        cMyTrace(__PRETTY_FUNCTION__,
            0x4886, 0)("CLEANPAD_POINT_CONTROL / m_id : %s , Point : %d (getPorint : %d)",
              NumberToString(user->get_acc_id(), 0),
              (unsigned int)pPkt->m_point,
              (unsigned int)user->m_mcap.getPoint());
    }
    return 0;
}