CSHA1::~CSHA1()
{
    // [P167 2026-09-11] ORIG 0x08087d4c（size 0x13、1 call）：D1 体内调 `CSHA1::Reset()`
    // —— 旧判「空析构」有误（p167 MISSING `_ZN5CSHA15ResetEv`）。按 ORIG 补调用。
    Reset();
}