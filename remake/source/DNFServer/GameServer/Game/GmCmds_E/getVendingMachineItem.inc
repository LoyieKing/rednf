void CRandomObjectTest::getVendingMachineItem(std::map<int, int>& out)
{
    stVendingMachineInfo_t* info = getVendingMachineInfo();
    if (info != 0)
    {
        Inven_Item item;
        unsigned long seed = static_cast<unsigned long>(time(NULL));
        // ORIG CMTRand 真类含 unsigned long 成员（对齐 4，rng 槽 -0xa48）；
        // 本仓占位类为 char[0x9c8]（对齐 1）⇒ 局部声明补对齐属性复现槽位。
        CMTRand __attribute__((aligned(4))) rng(seed);
        for (int i = 0; i < m_count; i++)
        {
            item.m_addInfo = -1;
            processVendingMachineItemLottery(rng, info, item);
            // ORIG：无 key 具名局部——find 实参为 unsigned 成员经转换临时
            // （-0x38）；insert 处重新读成员并 `(int)` cast 成 make_pair<int,int>
            // 的右值实参（-0x14 值临时）。
            std::map<int, int>::iterator it = out.find(item.m_addInfo);
            // P222 正向包裹：ORIG 块序 = it!=end → increment fall-through、
            // insert 走尾岛（`jmp` 后接 make_pair 段）。
            if (it != out.end())
            {
                it->second++;
            }
            else
            {
                out.insert(std::make_pair(static_cast<int>(item.m_addInfo), 1));
            }
        }
    }
    else
    {
        return;   // P285：空 else{return;} 保留 ORIG 循环出口 `jmp`+nop 岛
    }
