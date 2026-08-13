#ifndef NSL_INTERDISPATCHER_H_
#define NSL_INTERDISPATCHER_H_

namespace nsl {

class Message;
class IInterHandler;

class InterDispatcher
{
public:
    InterDispatcher() throw();
    bool dispatch(Message* msg);
};

} // namespace nsl

#endif // NSL_INTERDISPATCHER_H_
