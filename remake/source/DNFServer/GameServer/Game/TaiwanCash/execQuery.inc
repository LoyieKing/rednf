// ---- execQuery —— ORIG 0x081745f2。----
bool PurchaseQuery::execQuery(MySQL& db)
{
    return db.exec(true);
}