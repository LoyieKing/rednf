int ServerParameterScript::getRecoverStaminaPrice(int stamina, int price)
{
    int size = m_vec57c.size();
    int v = m_vec57c[std::min(size, stamina) - 1];
    // ORIG 0x0869375a：magic 0xb60b60b7 + sar $0x6 = 除以 90（非 100）
    return ((100 - price) * v) / 90;
