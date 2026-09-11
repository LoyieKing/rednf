// [b28d-20] _determine_level_bonus_index（ORIG 0x832cc02，64/63，nd 68）：
//   残差 = 帧 0x24(ORIG)/0x28(我方)+push ebx 有无 + idx 缓存寄存器（ORIG
//   `mov -0xc(%ebp),%ebx`）与 m_lvBonus 索引乘法展开（4.1 imul 链 vs 4.4
//   lea 链）；语义等价（RARITY 守卫与 while 短路链逐条对应）⇒ N7/N82 封顶。
// [b28d-solo-CItemList] DIFF（64/63）| 本轮修：合并 && → `while(size())`+
//   `if(rates[idx] >= baseLevel) break` 复现 ORIG 循环布局（短路第二项落 body、
//   第一项落尾条件；ndiff 75→45）；残 N7：idx 的 %ebx 跨 call 缓存
//   （ORIG `push %ebx` + `mov -0xc,%ebx`）与乘法链/`lea 0x18` vs `add $0x18`。
// [b28d-solo-CItemList] DIFF（64/63）| 本轮两形实测均反劣后封 N：
//   ① `register unsigned int idx` ⇒ idx 免槽（`mov $0,%ebx`）、帧 0x28→0x14，
//   与 ORIG 的 -0xc 槽往返不符；② 语句表达式实参 `[({ idx; })]` 零变化。
//   ORIG 0x0832cc2b `mov -0xc(%ebp),%ebx` 是 4.1 的提前装载（P75 跨版本调度），
//   4.4 恒在使用点 `mov -0xc,%eax` ⇒ 判 N（语义等价，p167 2/2 CLEAR）。
// [b28d-tu-CItemList 2026-09-09] 复核：65/64（gap −1）——残 = idx 的 %ebx 跨
//   call 保活（ORIG `push %ebx` + `mov -0xc(%ebp),%ebx`）与 4.4 使用点重载；
//   register 形实测 idx 免槽（帧 0x28→0x14）反劣，P231/P307 不适用 ⇒ N7/N82。
unsigned int CCompound_ExtreamDun_Item::_determine_level_bonus_index(  // ORIG T 0x832cc02
    unsigned int baseLevel, ENUM_INPUTITEMS_RARITY inRarity,
    eBindSphereType bind) const
{
    if ((int)inRarity < 0 || (int)inRarity > 2)
    {
        return 0;
    }
    // ORIG while 形：size()!=0 && v[idx]<baseLevel 短路链，两调用每次
    // 全地址重算（P48 无引用缓存）
    // [b28d-solo] P231 不适用：ORIG 0x832cc02 的 idx 占槽 -0xc（每轮
    //   `mov -0xc(%ebp),%ebx` 重载）——register 形实测免槽（`mov $0,%ebx`）反劣。
    // [b28d-solo-CItemList] P225/循环旋转：ORIG 4.1 把短路链**第二项**
    //   (rates[idx] >= baseLevel，setae 物化) 落在 body、第一项 (size()!=0)
    //   落在尾条件；4.4 对合并 && 恒反向展开（++idx 落 body）。改
    //   while(size())+if-break 复现 ORIG 布局。
    unsigned int idx = 0;
    while (m_lvBonus[(int)bind].m_rates[(int)inRarity].size() != 0)
    {
        if (m_lvBonus[(int)bind].m_rates[(int)inRarity][idx] >= baseLevel)
        {
            break;
        }
        ++idx;
    }
    return idx;
}