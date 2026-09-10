float CAutoMarketConditionsControl::GetItemDropRate(int levelRange)
{
    float rate = 1.0f;
    // [b28d-solo-LinkClosureB_Misc] P59/P257 正向包裹：ORIG `test; je <统一返回块>`
    //（m_field380==0 直落尾块，无 mov+jmp 提前返回岛）。
    if (m_field380 != 0)
    {
        if (!(static_cast<int>(m_vec3dc.size()) > levelRange && levelRange >= 0))
        {
            // [b28d-solo-LinkClosureB_Misc] P118/P139：ORIG 首参是 __PRETTY_FUNCTION__
            // 局部静态符号（_ZZ...E19__PRETTY_FUNCTION__）；格式串为单参版
            //（原重建多写 %s + 重复签名实参 = 真 bug）。
            cMyTrace trace(__PRETTY_FUNCTION__, 0xde, 0);
            trace("GetItemDropRate() error levelRange(%d)", levelRange);
            return rate;
        }

        // [b28d-solo-LinkClosureB_Misc] P183/P157：ORIG 两次独立 operator[]（p167
        // MISSING×1 实证）——原重建缓存 `int v` 少一次调用点。
        if (m_vec3dc[levelRange] != 0)
        {
            // ORIG fsubrp 实证 = `1.0 - m_equipProbBase`（非 base-1.0）。
            // ORIG fdivp 实证 = `(v*sub)/(-1000.0)`（非 -1000.0/(v*sub)，原重建
            // 除法方向整段写反 = 真语义 bug）。
            // 4.4 对标量 double 局部折叠为 `fmul mem`（少 1 条）；数组元素左值
            // 才发 ORIG 的 `fld mem; fmulp`（探针 p3.cpp f9）。
            double sub[1];
            sub[0] = 1.0 - m_equipProbBase;
            rate = static_cast<float>(
                1.0 + (m_vec3dc[levelRange] * sub[0]) / -1000.0);
        }

        // [b28d-solo-LinkClosureB_Misc] ORIG 符号名 _ZZ...E8logCount（P187）；
        // 后置自增形 `logCount++ % 1000 == 0` 才复现「先 sete 物化、后 lea 自增写回」。
        static int logCount;                   // ORIG b 0x0943bd78
        if (logCount++ % 1000 == 0)
        {
            // P0：ORIG 在 cMyTrace ctor 前先把 m_equipProbBase / (double)rate 各落具名
            // double 槽（-0x50/-0x48），trace 实参从槽读（非 ctor 后读成员）。
            double base = m_equipProbBase;
            double curRate = static_cast<double>(rate);
            cMyTrace trace(__PRETTY_FUNCTION__, 0xe8, 0);
            trace("itemDropRate(%f), levelRange(%d), curRate(%lf)",
                  curRate, levelRange, base);
        }
    }
    return rate;
