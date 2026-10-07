// _Z21getDamageRateCreatureiii @0x080955e1 (0x18d)：a=min(a,50)；sum=a+b；
// g_creature_damage_vector_ size>6 时 result = (d4 + d6/vec[6]) / 100，否则 1.0。
//   d1 = vec[1]*sum + vec[0] + c
//   d3 = vec[3]*sum + vec[2]
//   d4 = (d3/vec[4] + 1) * d1
//   d6 = vec[3]*sum + vec[5]
// [b28d-lane15 判词·N（N85 x87 整数↔浮点链）] → [C03CryptoInit68 修·2026-09-10]
// 按 P167 调用点差集修两处真差（本件原 `8×operator[]` vs ORIG `8×at()`，
// 及 int 乘积折 fildl）：
//  ① 全部改 `g_creature_damage_vector_.at(i)`（P175；ORIG 8 处 R_386_PC32 全为
//     `_ZNSt6vectorIiSaIiEE2atEj`）；
//  ② `(double)at(i) * (double)sum`（勿写 `(double)(at(i)*sum)`）⇒ 复现 ORIG
//     `fildl at; fildl sum; fmulp` 双 fildl；
//  ③ 加法的**先求值项**写在左侧（ORIG 恒先算 at(0)+c / at(2) / at(5) 并落槽），
//     即 `(at(0)+c) + at(1)*sum` 形（浮点加法可交换，P243 族）。
// 残余（跨版本族）：4.4 对内存操作数折叠 `faddl/fmull/fdivl mem`（ORIG 分步
// fldl+faddp，N85③）；double 临时槽 4.4 复用而 4.1 每步新槽（帧 0x68 vs 0x78，N82）。
// 语义等价，按 §6 记 N 封顶。
float getDamageRateCreature(int a, int b, int c)
{
    if (a > 0x32)
        a = 0x32;
    int sum = a + b;
    double result = 1.0;
    if (g_creature_damage_vector_.size() > 6)
    {
        // P175/P167：ORIG 8 处调用点全是 `vector::at(unsigned)`（非 operator[]）；
        // 逐点写 at()，勿缓存引用。
        // ORIG `fildl at(1); fildl sum; fmulp` ⇒ 两操作数各自 fildl（double 乘法），
        // 不是先做 int 乘法再 fildl（后者产 imul+mov+fildl）。
        // 且 ORIG 恒先求值「加法的右项」（at(0)+c / at(2) / at(5) 先落槽）⇒ 把该项
        // 写在加法左侧（浮点加法可交换，值不变）。
        double d1 = (static_cast<double>(g_creature_damage_vector_.at(0)) + static_cast<double>(c)) +
                    static_cast<double>(g_creature_damage_vector_.at(1)) * static_cast<double>(sum);
        double d3 = static_cast<double>(g_creature_damage_vector_.at(2)) +
                    static_cast<double>(g_creature_damage_vector_.at(3)) * static_cast<double>(sum);
        // [bughunt12] ORIG 0x080956d3..d9 `fildl at(4); fldl d3; de f1`(FDIVRP)
        // ⇒ d3 / at(4)（后载 / 先载）；旧形 at(4)/d3 方向反。
        double d4 = (d3 / static_cast<double>(g_creature_damage_vector_.at(4)) + 1.0) * d1;
        double d6 = static_cast<double>(g_creature_damage_vector_.at(5)) +
                    static_cast<double>(g_creature_damage_vector_.at(3)) * static_cast<double>(sum);
        // [bughunt12] ORIG 0x08095748 `fildl at(6); fdivrl d6` ⇒ d6 / at(6)；
        // 0x08095753..59 `fldl 100.0; de f9`(FDIVP ST1←ST1/ST0) ⇒ sum / 100.0。
        // 旧形 100/(d4 + at(6)/d6) 两处方向皆反。
        result = ((d4 + d6 / static_cast<double>(g_creature_damage_vector_.at(6))) / 100.0) * result;
    }
    return static_cast<float>(result);
}