bool GiftQuery::execQuery(MySQL& db)
{
    return db.exec(true);
}