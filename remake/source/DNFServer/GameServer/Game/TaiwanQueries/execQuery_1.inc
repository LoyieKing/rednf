// ORIG T 0x08174076
bool BalanceQuery::execQuery(MySQL& db)
{
    return db.exec(true);
}