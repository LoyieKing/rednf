int getItemSellPrice(int basePrice, int curEndurance, int maxEndurance, bool isRandomOption);

Store* G_Store()
{
    return GlobalInstance<Store>::inst_ptr();
