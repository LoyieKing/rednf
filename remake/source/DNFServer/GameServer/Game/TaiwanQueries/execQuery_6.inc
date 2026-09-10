bool ChargeQuery::execQuery(MySQL& db)
{
    return db.exec(true);
