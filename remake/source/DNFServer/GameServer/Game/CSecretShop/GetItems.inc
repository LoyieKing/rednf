bool SecretShopScript::GetItems(CMTRand* rand,
                                std::vector<secretshop::SALE_INFO>& out,
                                stDungeonSales& sales)
{
    // [b28d-solo-CSecretShop] P196/P0：ORIG 0x08a82d5e `cmp %edx,%eax; setb` 无符号序；
    // 0x08a82d58 的 SALE_INFO ctor 在**循环外**（sale 复用，旧实现每轮构造一次）；
    // 第二分支为 `for(; guard<=0x2710 && size<max; ++guard)` 尾条件形（ORIG
    // 0x08a82f35 `addl $1,-0xc` → 0x08a82f39 `cmpl $0x2710; jg` → 0x08a82f42 size
    // 检查），变量槽 seed(-0x1c ulong)/randVal(-0x14 uint)/acc(-0x10 uint)/guard(-0xc)。
    // 判词·N（本轮复核）：IsExistItem 判定改正向 goto 形消 4.4 的 `xor $1`
    // （ORIG 0x08a82eb4 `test %al,%al; jne <break 岛>`）⇒ 159→158，gap 0。
    // 残余 = N82（两分支 it/end 独立槽，全帧槽号浅移 0x20）+ setcc-fold
    // （ORIG 尾 `setb;test;jne` vs 4.4 `jae`+常量岛，P197④ 镜像负例）。
    if (sales.m_sales.size() < (unsigned int)sales.m_maxCount) {
        // 商品数少于上限：直接全部列出
        secretshop::SALE_INFO sale;
        for (std::map<int, stSaleInfo>::iterator it = sales.m_sales.begin();
             it != sales.m_sales.end(); ++it) {
            CopyItem(sale, it->second);
            out.push_back(sale);
        }
        // [M5D_SecretShop] ORIG 0x08a82dff `mov $0x1,%eax; jmp 08a82f68`（+0x224 为
        // 函数出口 leave/ret 前）+ else 结束后 0x08a82f63 另一处 `mov $0x1,%eax`
        // ⇒ 原码 if 分支**自带 return 1**（独立返回值物化），旧形与我方都是共享
        // 函数尾单块 ⇒ 少 1 条（P0 槽往返律的反例：4.1 不共享返回块）。
        return 1;
    }
    else {
        // 按权重随机抽取，直到填满 m_maxCount 或商品重复
        unsigned long seed = 0;      // -0x1c
        unsigned int randVal = 0;    // -0x14
        int guard = 0;
        // [试形记录] 与 IF 分支同名局部改独立名（sale2/it2）实测逐字节相同 ⇒ 4.4 的
        // 栈槽复用不随名字改变（N82：两分支对象各占独立槽不可复现，见函数头判词）。
        secretshop::SALE_INFO sale;
        for (; guard <= 0x2710 &&
               out.size() < (unsigned int)sales.m_maxCount; ++guard) {
            seed = (unsigned long)GetDomainRate(sales);
            randVal = (unsigned int)rand->randInt(seed);
            unsigned int acc = 0;
            for (std::map<int, stSaleInfo>::iterator it = sales.m_sales.begin();
                 it != sales.m_sales.end(); ++it) {
                acc += (unsigned int)it->second.m_rate;
                if (randVal < acc) {
                    // [M5D_SecretShop] ORIG 0x08a82eb4 `call IsExistItem; test %al,%al;
                    // jne 08a82f34`（跳 +0x1f0 = break 落点前的 nop 岛），CopyItem/
                    // push_back 段尾 0x08a82ef3 `jmp 08a82f35`（+0x1f1 同一落点）
                    // ⇒ 原码是**两条 break**（`if (f()) break; ...; break;`）。
                    // 旧形（正向 !=0 goto skip_copy）在 4.4 下生成 `je 标签; nop;
                    // jmp` 三条（+0x177 落点自带 nop），比 ORIG 多 2 条。
                    if (IsExistItem(out, it->second.m_itemIdx) != 0)
                        break;
                    CopyItem(sale, it->second);
                    out.push_back(sale);
                    break;
                }
            }
        }
    }
    return 1;
}