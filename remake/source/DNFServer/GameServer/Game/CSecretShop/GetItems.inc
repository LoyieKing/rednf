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
                    // [b28d-solo-CSecretShop] ORIG 0x08a82eb4 `call IsExistItem;
                    // test %al,%al; jne <break 落点>`（bool callee 直判、无 xor）。
                    // 4.4 对 `!f()`/`f()==0` 恒插 `xor $1`（P168a②/N122 实测）⇒
                    // 用正向条件 goto 复现 ORIG 的 `test;jne` 直分支形。
                    if (IsExistItem(out, it->second.m_itemIdx) != 0)
                        goto skip_copy;
                    CopyItem(sale, it->second);
                    out.push_back(sale);
skip_copy:
                    break;
                }
            }
        }
    }
    return 1;
