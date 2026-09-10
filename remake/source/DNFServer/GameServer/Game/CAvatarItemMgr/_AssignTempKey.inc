int CAvatarItemMgr::_AssignTempKey()
{
    // [b28d-solo-CAvatarItemMgr] 判词：NEAR（2 差）＝纯槽号差 ORIG counter@-0xc vs
    // 4.4 -0x4（帧 0x28 已对齐）；本轮再测 double framePad×1/×2、counter 先声明、
    // 8/0x10/0x18 数组维共 6 形，4.4 单标量恒落 -0x4 ⇒ N95/N82 槽基策略封顶。
    // ORIG 帧 0x28：P31 dummy local 撑帧（4.4 对 ≥16 字节数组保槽，标量死局部
    // 会被消除；探针 /tmp/slotprobe5.cpp 帧对齐实证）。残余 counter 槽 -0x4 vs
    // ORIG -0xc 为 4.1 单标量首槽策略差（探针 slotprobe6/7/8x/90/91 全败），
    // N82 族封顶不追。
    char framePad[0x10];
    (void)framePad;
    // [b28 复验] P190 double 撑槽与 P31 dead int 占槽两形均不把 counter 顶至
    // ORIG -0xc（4.4 标量首槽恒 -0x4，声明序/double 对齐/占位写全败）——N82 封顶。
    if (m_tempKeyCounter == 0x7fffffff)
        m_tempKeyCounter = 0;
    m_tempKeyCounter++;
    int counter = m_tempKeyCounter;   // ORIG -0xc 槽往返（读→存→读）
    // ORIG 0x082f9e02：成员读后经 -0xc 槽往返再传 _MakeTempKey（P0 具名局部），
    // 帧 0x28；4.4 折叠该 read→temp→read 往返且帧缩至 0x18/槽 -0x4 ⇒ N76-N82 梳。
    return _MakeTempKey(counter);
