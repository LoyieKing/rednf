bool ConfirmQuery::execQuery(MySQL& db)
{
    return db.exec(true);
