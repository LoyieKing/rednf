#ifndef NSL_DBDISPATCHER_H_
#define NSL_DBDISPATCHER_H_

namespace nsl {

class Message;

class DBDispatcher
{
public:
    DBDispatcher() throw();
    void dispatch(Message* pMsg);
};

} // namespace nsl

#endif // NSL_DBDISPATCHER_H_
