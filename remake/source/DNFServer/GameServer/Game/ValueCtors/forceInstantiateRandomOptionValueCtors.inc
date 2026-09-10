void forceInstantiateRandomOptionValueCtors()
{
    BaseItemSelectionValue a;
    BaseItemSelectionValue b(a);
    OptionGroupChoiceValue c;
    OptionGroupChoiceValue d(c);
    OptionGroupValue e;
    OptionGroupValue f(e);
    PartSelectonValue g;
    PartSelectonValue h(g);
    stCeraShopBonusInfo_t i;
    stCeraShopBonusInfo_t j(i);
    AradItemInfo k;
    // [b29-SymForceBatch] AradItemInfo l(k) 删除：ORIG 无 C1ERKS_/C2ERKS_
    // （拷贝 ctor 从未 ODR-use），隐式合成会发明符号。k 保留（ORIG 有
    // C1/C2/D1/D2 全 W 0x08515618/0x08514430）。
    (void)b; (void)d; (void)f; (void)h; (void)j;
