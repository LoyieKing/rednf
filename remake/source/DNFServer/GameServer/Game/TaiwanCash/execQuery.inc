bool PurchaseQuery::execQuery(MySQL& db)
{
    return db.exec(true);
