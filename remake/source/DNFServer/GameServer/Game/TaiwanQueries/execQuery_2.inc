bool BalancePointQuery::execQuery(MySQL& db)
{
    return db.exec(true);
