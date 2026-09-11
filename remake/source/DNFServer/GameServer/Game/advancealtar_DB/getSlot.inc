// ORIG T 0x08899b4e：把 map<uchar,_Slot> 拷入定长槽数组（槽 0 长度 10）。
void AdvanceAltarData::getSlot(const std::map<unsigned char,_Slot>& m, _Slot* out, int count) const
{
    int local_10 = 0;
    for (std::map<unsigned char,_Slot>::const_iterator it = m.begin();
         it != m.end() && local_10 < count; )
    {
        // [b28d-solo-advancealtar_DB] AE（orig 83 / ours 83，clean resid=0）。
        //   语义（je 条件码逐条推导）：ORIG 0x08899b85 `test eax,eax; sete %al;
        //   test %al,%al; je <+0x5d>` —— sete 后 test 使 ZF=(x!=0)，je 在 x!=0 时
        //   跳第二检查 ⇒ **x==0 才 ++it**（勿按 setcc 字面反推）。
        //   归零钥匙：① 第一检查 `==0` 直写（ORIG sete 形）；② 第二检查按 P222
        //   正极性 `!=0` 写（ORIG 0x08899bbf je 落 memcpy）——单独改任一处各留残差。
        //   P183：ORIG 无具名 `_Slot& slot` 槽（每次 ptEv 重算）⇒ 直接写
        //   `it->second.m_int1`；P28：增量段调 ppEi ⇒ 源形必写后置 `it++`。
        if (it->second.m_int1 == 0)          // 源槽空 → 跳过
        {
            it++;
        }
        // P222 正极性翻转（语义不变）：ORIG 0x08899bbf `test; je <memcpy>` ——
        // 目标非空走 ++local_10 分支，写成 `!=0` 正极性才产 je 落 memcpy。
        else if (out[local_10].m_int1 != 0)  // 目标槽非空 → 前进目标下标
        {
            ++local_10;
        }
        else
        {
            std::memcpy(&out[local_10], &it->second, 10);
            it++;
            ++local_10;
        }
    }
}