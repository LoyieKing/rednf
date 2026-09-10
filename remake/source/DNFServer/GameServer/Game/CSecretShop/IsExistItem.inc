bool SecretShopScript::IsExistItem(std::vector<secretshop::SALE_INFO>& out,
                                   int itemIdx)
{
    // [b28d-56 判词·AE] ORIG 0x08a82f6a 为 std::find：begin→-0x14、end→-0x10、
    // find sret→-0x18、end→-0xc 四槽（P96 非 const iterator，P293 find 先/end 后），
    // 尾 `test %al; je; mov $1; jmp; mov $0`。旧索引 for 循环仅 28 条、缺全部
    // 迭代器调用（43 vs 28）。
    std::vector<secretshop::SALE_INFO>::iterator it =
        std::find(out.begin(), out.end(), itemIdx);
    if (it != out.end())
        return true;
    return false;
