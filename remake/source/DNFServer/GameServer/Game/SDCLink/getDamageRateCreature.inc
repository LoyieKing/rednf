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
        double d4 = (static_cast<double>(g_creature_damage_vector_.at(4)) / d3 + 1.0) * d1;
        double d6 = static_cast<double>(g_creature_damage_vector_.at(5)) +
                    static_cast<double>(g_creature_damage_vector_.at(3)) * static_cast<double>(sum);
        result = (kDamageRateDiv100 / (d4 + static_cast<double>(g_creature_damage_vector_.at(6)) / d6)) * result;
    }
    return static_cast<float>(result);
