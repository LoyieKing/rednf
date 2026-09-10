void generateDropAvatarItem(const stEnamyDropAvatarInfo_t& info,
                            unsigned char rarity, unsigned char grade,
                            std::vector<Inven_Item>& out)
{
    // ① 入门阈值（ORIG fildll/flds/fmulp/fistpll 序列：64 位整数域 + float 乘法，
    //    表引用与 base 指针内联 —— ORIG 每访问点 `mov 0x8(%ebp),%eax` 重读参数，
    //    无 base/tableA/tableB 具名槽）。
    long long thresholdLL =
        static_cast<long long>(static_cast<unsigned int>(info.m_field00));
    thresholdLL = static_cast<long long>(
        static_cast<float>(thresholdLL) *
        *reinterpret_cast<const float*>(
            reinterpret_cast<const char*>(&info) + 0x8 +
            4 * (static_cast<unsigned>(rarity) + 4)) *
        *reinterpret_cast<const float*>(
            reinterpret_cast<const char*>(&info) + 0x8 +
            4 * (static_cast<unsigned>(grade) - 1)));
    const int threshold = static_cast<int>(thresholdLL);
    if (get_rand_int(100000) > threshold)
        return;

    // ② roll 轮次
    for (unsigned int round = 0; round < (unsigned int)info.m_field04; ++round)
    {
        const int r = get_rand_int(info.m_field2c);
        std::vector<stEnamyDropAvatarElement_t>::const_iterator it =
            info.m_elements.begin();
        while (it != info.m_elements.end())
        {
            if (static_cast<unsigned int>(it->m_field4) >
                static_cast<unsigned int>(r))
            {
                Inven_Item item;
                item.m_addInfo = it->m_field0;              // ORIG 写 +0x02 视图
                G_CDataManager()->m_itemList->create_item(
                    (ENUM_ITEM_CREATE_TYPE)1, item, 1);
                out.push_back(item);
                break;                                      // 不推进迭代器（ORIG jmp +0x170）
            }
            it++;
        }
    }
