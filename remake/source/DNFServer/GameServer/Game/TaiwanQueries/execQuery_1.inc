bool BalanceQuery::execQuery(MySQL& db)
{
    return db.exec(true);
