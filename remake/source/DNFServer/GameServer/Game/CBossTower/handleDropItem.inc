// ORIG T 0x08143e5e：转发 CBossStage::handleDropItem，结果 ==1 → 1 否则 0。
bool CBossTower::handleDropItem(CUser* user, int a, int b, char c, int d, int e)
{
    bool ret = CBossStage::handleDropItem(user, a, b, c, d, e);
    // ORIG 0x08143ea3 movzbl+xor$1+test/je：bool 槽取反物化（P158 形）。
    if (!ret)
        return 0;
    return 1;
}