bool SecretShopScript::GetItemByEvent(CMTRand* rand,
                                      std::vector<secretshop::SALE_INFO>& out)
{
    std::vector<secretshop::SALE_INFO> list;
    // [b28-L14] P0 清绑（原 `register unsigned char fv asm("eax")` 全域禁用）。
    // ORIG 0x8a82c70：`call GetItems; xor $0x1,%eax; test %al,%al;
    // je <主路径>` + fall-through `mov $0,%ebx` ⇒ 裸否定早返形（P168a①）。
    if (!GetItems(rand, list, m_eventDungeonSales))
        return false;
    for (int i = 0; i < static_cast<int>(list.size()); ++i) {
        list[i].m_field18 = 1;
        out.push_back(list[i]);
    }
    return true;
}