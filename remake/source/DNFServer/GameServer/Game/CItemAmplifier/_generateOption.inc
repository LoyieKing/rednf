// [b28d-solo-CItemAmplifier] verdict=NEAR（115/115，归一 diff 40）：N82 族封顶。
//   4.4 把具名 float 局部放最浅槽（-0xc/头部声明 -0x14）而 ORIG 放最深 -0x24，
//   连带 it/end/temp/fpucw 槽组 -4..-0x10 漂移、帧 0x48 vs 0x38；三形实测
//   （if 内声明 -0xc/48、头部声明 -0x14/40、头部带初始化 -0x14/42）均不可达
//   ORIG 槽位。已修：outOfRange 去具名 bool（P0）、rand2 移到 begin/end 之后
//   （ORIG 0x08515031）、m_type 走同宽 reinterpret 免 int→enum 中转。
void CItemAmplifier::_generateOption(const CItem* item,
                                     ENUM_AMPLIFY_TYPE& type,
                                     unsigned short& value) const
{
    float baseValue;   // [b28d-solo-CItemAmplifier] 头部先声明（ORIG 槽 -0x24 最深）
    // ORIG 0x08514f76..0x08514fa3：范围守卫**两次独立 get_rarity**（0x08514f7c
    // 与 0x08514f8b）+ `mov$1/mov$0` 汇聚岛 + `test%al; je`，且 bool 值**无槽
    // 往返**（P0）⇒ 原码是表达式右值（`||` 前端物化），非具名 bool 局部；
    // 旧 `const bool outOfRange` 形多 `mov %al,-0x15; cmpb` 两步 + 帧 +0x10。
    if (item->get_rarity() < 0 || item->get_rarity() > 5)
    {
        type = ENUM_AMPLIFY_TYPE_NONE;
        value = 0;
        return;
    }

    int rand = get_rand_int(0x2710);
    if (m_script.m_rarityProb[item->get_rarity()] <= rand)
    {
        type = ENUM_AMPLIFY_TYPE_NONE;
        value = 0;
        return;
    }

    std::vector<stAmplifyOptionInfo_t>::const_iterator it =
        m_script.m_options.begin();
    std::vector<stAmplifyOptionInfo_t>::const_iterator end =
        m_script.m_options.end();
    // ORIG 0x08515031：rand2 在 begin()/end() **之后**求值（槽 -0xc 浅于 it/end）。
    int rand2 = get_rand_int(0x2710);
    while (it != end)
    {
        if (it->m_prob > rand2)
        {
            // [b28d-solo-CItemAmplifier] ORIG 0x0851506c `mov (%eax),%edx` 单段
            // 直载：m_type 为 int 字段，`(ENUM_AMPLIFY_TYPE)it->m_type` 转换在
            // 4.4 多一条 `mov %eax,%edx`（P254 反向）；改用同宽 reinterpret 视图
            // 免转换（m_type 类型不动，investAmplifyOption 的 int 比较段不受影响）。
            type = reinterpret_cast<const ENUM_AMPLIFY_TYPE&>(it->m_type);
            // ORIG 0x0851508x：先 `fildl; fstps -0x24` 把 m_value 落成 float 槽，
            // 再**第三次** get_rarity → `flds m_rarityRate[r]`（被乘数在前）→
            // `fmuls -0x24` → 控制字 ROUND → `fistpl`。故 rarityRate 作左操作数、
            // m_value 走具名 float 局部（P176 ROUND 辨伪 + P183 重物化）。
            baseValue = (float)it->m_value;
            value = (unsigned short)(
                m_script.m_rarityRate[item->get_rarity()] * baseValue);
            return;
        }
        ++it;
    }
}