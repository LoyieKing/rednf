#ifndef NSL_SENDBUFFER_H_
#define NSL_SENDBUFFER_H_

namespace nsl {

struct SendBuffer
{
    char buf[10240];
};

struct DbBuffer
{
    char buf[1024];
};

} // namespace nsl

#endif // NSL_SENDBUFFER_H_
