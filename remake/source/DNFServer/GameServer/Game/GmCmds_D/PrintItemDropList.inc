// ORIG PrintItemDropList 0x084b1756：单条独立掉落统计文本追加到 out。
// 【b28d-solo-GmCmds_D 判词 N78/N82/P167】`={0}` rep stos 替代 ORIG 的内联
// memset 慢路（N78）；`if(!detail)` 反形（P168a②）、rate 第一次内联/第二次具名
// float（P252）、内层 begin/end 缓存（P13）、segRate 内联均已按 ORIG 复现。
// p167：EXTRA operator-> ×2 / MISSING operator* ×1（ORIG 单次 `*pit` + 内联 pair
// 拷贝；改用 pair 局部虽 p167 CLEAR 但会丢 ORIG 存在的 operator-> 弱符号
// 0x08193912 使 TU identical −1，故取当前形）。残差另含全帧槽号错位（N82）。
void CIndependentDrop::PrintItemDropList(
    const stMonsterIndependentDrop_t& drop, char* out, bool detail)
{
    // b28d-solo-GmCmds_D：ORIG 用内联 rep stos 清零（P214③/P253）⇒ `={0}` 形。
    char lineBuf[0x400] = {0};                               // -0x42c

    // ORIG `movzbl -0x43c,%eax; xor $1; test %al,%al; je <memcpy 块>`（P168a②）⇒
    // 源码为 `if (!detail) { find } else { memcpy }`。
    if (!detail)
    {
        CItem* item = G_CDataManager()->find_item(drop.m_itemIdx);
        if (item != 0)
        {
            sprintf(lineBuf, "%d\t%s\n", drop.m_itemIdx, item->GetItemName());
            strcat(out, lineBuf);
        }
    }
    else
    {
        memcpy(lineBuf, "\xb8\xae\xbd\xba\xc6\xae \xb5\xe5\xb6\xf8\n", 0xd);
        strcat(out, lineBuf);
    }

    for (int i = 0; i < 5; ++i)
    {
        if (drop.m_rate[i] == 0)
        {
            continue;
        }
        sprintf(lineBuf, "\xb3\xad\xc0\xcc\xb5\xb5\t%d\n", i);
        strcat(out, lineBuf);

        // ORIG 第一次 rate 无 float 槽（fildl/flds/fdivrp/fstpl 直落实参）⇒ 内联
        // 表达式；第二次 `fstps -0x14` 具名 float 局部供 4 次乘法（P252）。
        sprintf(lineBuf, "\xb5\xe5\xb6\xf8\xc0\xb2(%%)\t%f\n",
                static_cast<double>(
                    static_cast<float>(drop.m_rate[i]) / 10000.0f));
        strcat(out, lineBuf);

        sprintf(lineBuf, "\xbd\xc3\xb5\xb5\xbc\xf6\t%d\t%d\t%d\t%d\n",
                drop.m_expect[0], drop.m_expect[1],
                drop.m_expect[2], drop.m_expect[3]);
        strcat(out, lineBuf);

        const float rate =
            static_cast<float>(drop.m_rate[i]) / 10000.0f;   // ORIG fstps -0x14
        sprintf(lineBuf, "\xc3\xd1 \xb5\xe5\xb6\xf8\xc0\xb2(%%)\t%f\t%f\t%f\t%f\n",
                static_cast<double>(static_cast<float>(drop.m_expect[0]) * rate),
                static_cast<double>(static_cast<float>(drop.m_expect[1]) * rate),
                static_cast<double>(static_cast<float>(drop.m_expect[2]) * rate),
                static_cast<double>(static_cast<float>(drop.m_expect[3]) * rate));
        strcat(out, lineBuf);

        sprintf(lineBuf, "\xc3\xd1 \xb0\xb3\xbc\xf6\t%d\n\n", drop.m_limit);
        strcat(out, lineBuf);

        if (detail)
        {
            memcpy(lineBuf, "\xbe\xc6\xc0\xcc\xc5\xdb \xb8\xae\xbd\xba\xc6\xae\n", 0xf);
            strcat(out, lineBuf);

            int prevAcc = 0;                                 // -0x10
            // ORIG 循环前一次性求值 begin(-0x24)/end(-0x20)（P13）。
            std::vector<std::pair<int, int> >::const_iterator pit =
                drop.m_pairList.begin();
            std::vector<std::pair<int, int> >::const_iterator pend =
                drop.m_pairList.end();
            for (; pit != pend; ++pit)
            {
                // [b28c-R26 复验] 已知取舍：ORIG 0x084b1a68 用 `*pit`（deEv，0x08235bba）
                // 且**不调** ptEv；写 `pit->first/pit->second` 会 p167 EXTRA ptEv ×2 /
                // MISSING deEv ×1（实测 calls 29 vs 30）。改用 `const std::pair<int,int>
                // p = *pit;` 可得 p167 CLEAR（29=29 实测），但**不再发射*** ORIG 实有
                // 的 `_ZNK9__gnu_cxx17__normal_iteratorIPKSt4pairIiiE...EptEv` 弱符号
                // 0x08193912 ⇒ TU identical 95→94（净负，实测）。故维持 `->` 形。
                // [round5-GmCmds_D 负结论·复测确认] pair 值拷贝形
                // `const std::pair<int,int> p = *pit;` 实测：指令数 261→255（=ORIG
                // 255，p167 CLEAR 29=29），但 **TU 四数 identical 95→94、合计
                // 253→252**（ptEv 弱符号实例不再发射 ⇒ ORIG 实有而本 TU 缺失的
                // 条目 −1）⇒ 按「四数不劣化」闸门**回滚**，维持 `pit->` 双 ptEv 形。
                const int itemId = pit->first;
                const int acc = pit->second;
                CItem* item = G_CDataManager()->find_item(itemId);
                if (item != 0)
                {
                    // ORIG segRate 无槽（fildl/flds/fdivrp 直落 sprintf 实参）。
                    sprintf(lineBuf, "%d\t%f\t%s\n", itemId,
                            static_cast<double>(
                                static_cast<float>(acc - prevAcc) / 10000.0f),
                            item->GetItemName());
                    strcat(out, lineBuf);
                    prevAcc = acc;
                }
            }
            prevAcc = 0;
        }
    }
}