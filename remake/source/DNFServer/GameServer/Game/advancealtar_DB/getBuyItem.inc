// ORIG T 0x08899a3a：把 map<int,_BuyItem> 拷入定长购买项数组（每项 12）。
// [b28d-solo-advancealtar_DB] AE（orig 83 / ours 83，clean resid=0）。同 getSlot：
//   语义 ORIG 0x08899a75 `test; setle %al; test; je <+0x5d>` ⇒ **x<=0 才 ++it**。
//   归零钥匙：① 第一检查 `< 1` 直写（setle 形）；② 第二检查按 P222 正极性 `>0`
//   写（ORIG 0x08899ab0 jle 落 memcpy）。两处联动才归零。
void AdvanceAltarData::getBuyItem(const std::map<int,_BuyItem>& m, _BuyItem* out, int count) const
{
    int local_10 = 0;
    for (std::map<int,_BuyItem>::const_iterator it = m.begin();
         it != m.end() && local_10 < count; )
    {
        // [b28d-solo-advancealtar_DB] 语义核对（同 getSlot）：ORIG 0x08899a75
        //   `test; setle %al; test; je <+0x5d>` ⇒ x<=0 才 ++it（setle 后 test 使
        //   ZF=(x>0)，je 在 x>0 时跳第二检查）。第二检查 0x08899ab0
        //   `test; jle <memcpy>` ⇒ 目标 m_field4<=0 才写入。P183/P28 同上。
        if (it->second.m_field4 < 1)          // 源项无效 → 跳过
        {
            it++;
        }
        // P222 正极性翻转（语义不变）：ORIG 0x08899ab0 `test; jle <memcpy>`。
        else if (out[local_10].m_field4 > 0)  // 目标项有效 → 前进目标下标
        {
            ++local_10;
        }
        else
        {
            std::memcpy(&out[local_10], &it->second, 0xc);
            it++;
            ++local_10;
        }
    }
}