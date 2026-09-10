bool CParty::isDungeonDifficultyInfromNotice(bool flag)
{
    // [b28d-59 判词] 封 N（P292/P248档① 跨 TU bool 链，ORIG 0x085be8a0）：49/47 条，
    // 残差首条=ORIG `call getMemberDungeonFlag; test %al; je`（bool callee）vs 我方
    // `setne %al`（CUserCharacInfo.h:923 声明 unsigned char）。整链同改需动
    // CUserCharacInfo.h/.cpp（非本泳道 TU）⇒ 记 N 待属主同步；其余差异同源。
    // this+0x74 = m_manager（CUserCharacInfo 基类方法直呼）
    if (m_manager->getMemberDungeonFlag())           // W 0x085bfaec（bool 直判：ORIG test;je 单物化）
        return false;
    if (m_manager->getCurCharacTutorialFlag() == 0)  // W 0x085bfb44：新手教程
        return false;                                // 未完成（==0）不提示
    if (flag)                                // param_1 为真直接不提示
        return false;
    // this+0xc4c = CBattle_Field(+0xb24)+0x128 = m_villageMonsterId(short)。
    // 直接成员访问，编译器以 this 基址直寻 0xc4c（ORIG `movzwl 0xc4c(%eax)`）。
    if (m_battleField.m_villageMonsterId != 0)
        return false;
    if (getManager()->getMoveSpace() == (UserSpace::T)1)   // getMoveSpace 0x08690eca
        return false;
    return true;
