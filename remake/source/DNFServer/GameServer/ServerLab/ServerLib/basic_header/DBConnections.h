#ifndef NSL_DBCONNECTIONS_H_
#define NSL_DBCONNECTIONS_H_

#include <map>

namespace nsl {

class DBConnection;

class DBConnections
{
public:
    DBConnections()
    {
    }
    ~DBConnections()
    {
    }
    DBConnection* getDBConnection(int idx)
    {
        return mDbConnections[idx];
    }
    void setDBConnection(int idx, DBConnection* pConn)
    {
        mDbConnections[idx] = pConn;
    }

    std::map<int, DBConnection*> mDbConnections;
};

} // namespace nsl

#endif // NSL_DBCONNECTIONS_H_
