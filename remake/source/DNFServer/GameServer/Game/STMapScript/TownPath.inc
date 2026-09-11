// ---------------------------------------------------------------------------
// TownPath —— 切片实证 TownPath 以 MapArea 为子对象（基类），本 TU 四个方法
// 全部委托给基类：clear 0x089e8258→call _ZN7MapArea5clearEv 0x08366050、
// C1 0x089e828e→call _ZN7MapAreaC1Ev + vector<int>::C1@0x24 后 call 自身 clear、
// C1ERKS 0x083d9b78→call _ZN7MapAreaC1ERKS_、aSERKS_ 0x089ea970→call
// _ZN7MapAreaaSERKS_；随后 +0x1c/+0x20 两 int 与 vector<int>@0x24。
// ---------------------------------------------------------------------------
TownPath::TownPath()                        // ORIG 0x089e828e
{
    clear();
}