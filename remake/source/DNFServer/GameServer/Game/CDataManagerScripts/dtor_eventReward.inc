// [b28d-solo-CDataManagerScripts] AE：map D1 + CSlotBoundChecker D1（+0x18 真实内嵌成员，ORIG 0x0837dfb2 调 _ZN17CSlotBoundCheckerD1Ev）；fresh check_tu_routed clean（27=27）；ORIG 0x0837dfb2。
eventReward::~eventReward()
{
    // ORIG D1 0x837dfb2：析构 map + CSlotBoundChecker（后者静态实现无成员，跳过）
}