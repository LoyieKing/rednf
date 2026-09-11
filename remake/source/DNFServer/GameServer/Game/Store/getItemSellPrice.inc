// getItemSellPrice 的唯一实现归 ItemUpgradeVtableB2.cpp（ORIG 0x0898c86e）；
// 本 TU 的 Store::GetSellItemPrice 仅调用该共享自由函数。
int getItemSellPrice(int basePrice, int curEndurance, int maxEndurance, bool isRandomOption);

Store* G_Store()
{
    return GlobalInstance<Store>::inst_ptr();
}