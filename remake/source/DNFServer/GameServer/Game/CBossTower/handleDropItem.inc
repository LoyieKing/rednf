bool CBossTower::handleDropItem(CUser* user, int a, int b, char c, int d, int e)
{
    bool ret = CBossStage::handleDropItem(user, a, b, c, d, e);
    // ORIG 0x08143ea3 movzbl+xor$1+test/je：bool 槽取反物化（P158 形）。
    if (!ret)
        return 0;
    return 1;
