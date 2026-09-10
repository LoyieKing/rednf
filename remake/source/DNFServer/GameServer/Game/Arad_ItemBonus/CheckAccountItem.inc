bool CheckAccountItem(CUser* user, unsigned int itemIndex,
                      class PurchaseLimitItem::Info& out)
{
    (void)user;  // ORIG 不使用 user 形参（Ghidra/汇编均无引用）
    // P0：ORIG 无 mgr 局部（Get→getPurchaseLimitItem→find 寄存器直传）；
    // 尾部 choose 岛（test;je;mov$0;jmp;mov$1）＝ P56 if-return 形。
    if (Singleton<Arad_DataManager>::Get()->getPurchaseLimitItem()->find(itemIndex, out))
        return false;
    return true;
