int GetRoundInt(float v)
{
    // [b28d-solo-ScriptDepsA 判词] AE（Main 已落路由）：dnf_game_cxx.sh i486 组
    // 纳入本 TU 后，fucompp/fnstsw %ax/sahf 两向比较与 ORIG 0x08a3df64 全等。
    // 双后端读数：默认 707/17 → i486 708/16，本件 DIFF→AE，零回归。
    if (v >= 0.0f) return static_cast<int>(v + 0.5f);
    return static_cast<int>(v - 0.5f);
