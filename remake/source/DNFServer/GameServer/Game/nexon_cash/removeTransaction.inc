void UserBalance::removeTransaction(Transaction* tx)
{
    // [b28d-solo-nexon_cash] P23/P75：ORIG 0x081b1530 求值序 = 外层 erase 的
    // end()(-0x18) → 内层 remove 的 end()(-0x10)/begin()(-0xc) → remove →
    // erase；具名 last 两句式令 4.4 求值序 end/begin/remove/end 反形 ⇒ 必须
    // 写单表达式嵌套形。
    m_transactions.erase(
        std::remove(m_transactions.begin(), m_transactions.end(), tx),
        m_transactions.end());
