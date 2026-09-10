int ServiceFactory::shutdown()
{
    if (this != NULL)
    {
        std::map<int, DBConnection*>::iterator dbConnIter;
        dbConnIter = super_DBConnections.mDbConnections.begin();
        for (; dbConnIter != super_DBConnections.mDbConnections.end(); ++dbConnIter)
        {
            dbConnIter->second->close();
        }
    }
    /* ORIG: no explicit mov $0 before leave; return value is last ne() result (0) */
