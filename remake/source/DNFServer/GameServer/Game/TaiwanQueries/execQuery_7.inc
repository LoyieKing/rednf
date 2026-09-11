bool CeraPointQuery::execQuery(MySQL& db)
{
    return db.exec(true);
}