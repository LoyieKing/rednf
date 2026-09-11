// ---- CStage ----
// ORIG W 0x82a74ca：MapInfo ctor（编译器生成）后 resetCurrentStage()。
// [b28d-solo-CDeathTower] 去 throw()（Main 代落头 CDeathTower.h:88/91 同步）：
// ORIG C1 landing 0x82a763f 只调 ~CDungeonMgr（= CStage ctor 抛出时的 cleanup）
// ⇒ ORIG 的 CStage ctor/dtor 可抛，加 throw() 会把该 landing 一并消掉（净负）。
CDeathTower::CStage::CStage()
{
    resetCurrentStage();
}