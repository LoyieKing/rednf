bool CancelQuery::execQuery(MySQL& db)
{
    return db.exec(true);
}