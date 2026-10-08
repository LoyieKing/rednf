// ---------------------------------------------------------------------------
// GiveMileage —— ORIG T 0x0819dc9c（报告 GiveMileage.md 全文转写）。
// 汇编算术：fild rate; fild amount; flds [1000.0f @0x8ba7330]; fdivrp; fmulp;
//   fstpl; call floor; fistp（fldcw 0xc 截断模式）→ (int)floor(...)。
// ---------------------------------------------------------------------------
// [b28d-solo-Arad_IPGProcess] IDENTICAL：三条钥匙——①`int rate=0` 前置清零 +
// `int found;` 两句式（-0x14/-0x10 槽序按声明序 rate→found 复现）；②Get 链全程右值
// 直传（P0，无具名 mgr 局部）；③ROUND 结果具名 `int mileage` 落 -0xc，x87 左操作数
// 写 `(float)amount * ((float)rate / 1000.0f)` 复现 fild amount→fild rate 序（P243）。
// 51=51 逐条全等（fresh .o 核验）。
void Arad_MileageProcess::GiveMileage(CUser* user, int ipgNo, int amount)
{
    if (user == 0)
        return;

    // ORIG -0x14（rate）前置清零、-0x10（found）独立槽；Get 链全程右值直传（P0）。
    int rate = 0;
    int found;
    found = ARAD::Singleton<ARAD::Arad_DataManager>::Get()
                ->getAradMileageData()->find(ipgNo);
    if (found >= 0 && found <= 1000)
        rate = found;
    else
        rate = ARAD::Singleton<ARAD::Arad_DataManager>::Get()
                   ->getAradMileageData()->default_value();

    // ORIG x87 序：fild amount → fild rate → flds 1000.0f → fdivrp → fmulp
    // ⇒ 左操作数 amount 先求值；ROUND 结果落具名 int 槽 -0xc。
    // [DNF-FIX 2026-10-08 c26] ORIG fstpl (%esp); call floor@plt ⇒ ::floor(double)；g++13 <cmath> 下 float 实参会落到 std::floor(float)。
    int mileage = (int)floor((double)((float)amount * ((float)rate / 1000.0f)));
    user->AddMileage(mileage, (eCoinAddReason)3);
}