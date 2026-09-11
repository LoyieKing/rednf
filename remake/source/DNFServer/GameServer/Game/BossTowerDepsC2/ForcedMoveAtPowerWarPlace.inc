// ============================================================================
// CAssaultPlace::ForcedMoveAtPowerWarPlace —— ORIG T 0x082ed6a6（静态成员，
// asm 无 this 槽）。报告全文转写：IsPowerWarEventOn() 且村庄==7 才处理；
// getPowerSide 两次独立调用（ORIG 指令流如此）；非 1/2 时其余字段不写
// （保持未初始化栈值，与 ORIG 一致）。
// ============================================================================
// [b28d-L8][W6] P226：ORIG 短路 bool 无槽物化（%eax 直传 + test %al,%al; je 尾）
// ⇒ 用语句表达式包住整个 && 条件（内不含具名变量），消掉 int enter 落槽
// （旧形帧 0x48 vs ORIG 0x38）。ORIG 0x082ed6a6。
void CAssaultPlace::ForcedMoveAtPowerWarPlace(CUser* user)
{
    if (({ GlobalData::s_power_manager->IsPowerWarEventOn() &&
           user->getCurCharacVill() == 7; }))
    {
        CForcedMoveCharacter request;   // ORIG 栈对象，无构造调用
        request.m_user = user;
        if (user->getPowerSide() == 1)
        {
            request.m_destVill = 7;
            request.m_destArea = 0;
            request.m_x = 0x255;
            request.m_y = 300;
        }
        else if (user->getPowerSide() == 2)
        {
            request.m_destVill = 7;
            request.m_destArea = 4;
            request.m_x = 0x139;
            request.m_y = 0x13a;
        }
        request.ForcedMove();
    }
}