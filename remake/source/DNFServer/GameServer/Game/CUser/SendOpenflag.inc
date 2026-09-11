// 0x086568fc
// ORIG：开放标志包(0xb1)。角色存在时：每日币事件(4)进行中取等级 1 的当日币数，
// premium 有优势加币加成；计算缺口（need - coin）并在 opencoin 标志下发包。
// b28d-lane5 N封顶: P11 符号扩展; N82/P112 槽号分配 | ORIG 0x086568fc | _ZN5CUser12SendOpenflagEv
void CUser::SendOpenflag()
{
    // 2026-09-10 gA（A 组）负结论 N（131 vs 133）：ORIG 0x086568fc 主体逐条同形
    //   （GetRepeatEvent 具名槽 -0xc + `cmpl $0x0,-0xc; je` 判空、两处局部 0 预置
    //   -0x14/-0x10、getCurCharacInvenR/GetCoin 无具名槽），残差为我方全帧槽号 -4 与
    //   +2 条搬运/对齐 ⇒ 槽分配/布局族，无源码级缺口可下刀，N 封顶。
    if (getCurCharacR() == 0)
        return;
    unsigned int coinNeed = 0;
    int lack = 0;
    CCoinEventPerDay* pEvent =
        (CCoinEventPerDay*)GlobalData::s_event_manager->GetRepeatEvent(4);
    if (pEvent != 0 && pEvent->IsEventing((CUser*)0) != 0)
        coinNeed = (unsigned int)pEvent->GetCoinNoPerDay(1);
    if (IsHavePremiumAdvantage())
        // ORIG 0x086568fc：GetPremiumInfo() 转发后 GetAdvantageCoin，不直读 m_premium
        coinNeed = coinNeed + (unsigned int)GetPremiumInfo()->GetAdvantageCoin();
    // 2026-09-11 CuB（P0 槽-局部双向对应律）：ORIG 0x086569c4 `call CInventory::GetCoin`
    //   的结果**直接** `cmp -0x14(%ebp),%eax`（无槽往返）⇒ 原码该处为表达式直读，
    //   我方旧形 `unsigned int coin = getCurCharacInvenR()->GetCoin();` 多出
    //   store+load 一对（= 我方多 2 条、全帧 +4 的唯一来源）⇒ 删具名、内联回条件位。
    //   （else 臂 ORIG 0x086569f1 重调 getCurCharacInvenR/GetCoin ⇒ 原码两次直读，保持。）
    if (!(getCurCharacInvenR()->GetCoin() < coinNeed && GetOpencoin() != (char)1))
    {
        lack = 0;
    }
    else
    {
        lack = (int)(coinNeed - getCurCharacInvenR()->GetCoin());
    }
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0xb1);
    // 2026-09-11 L9GrpF（P9/P11 类型轴）：ORIG 0x08656a3c `call GetOpencoin; movsbl %al,%eax`
    //   = 调用点按 **signed char** 扩宽（我方 `(int)` 只出 movzbl）。GetOpencoin 声明维持
    //   CUserCharacInfo.h:1118 `unsigned char`（非本 TU 写域）⇒ 在调用点加 `(char)`；
    //   值域 0..0xff、put_byte 仅取低字节 ⇒ 行为不变（探针 psi.cpp f0/f1 实证符号扩展形）。
    ((InterfacePacketBuf*)&packet)->put_byte((int)(char)GetOpencoin());
    ((InterfacePacketBuf*)&packet)->put_int(lack);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
}