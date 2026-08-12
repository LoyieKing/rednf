#ifndef DNF_INNER_MSG_HANDLER_H_
#define DNF_INNER_MSG_HANDLER_H_

// ---- CInnerMsgHandler：空类 ----
class CInnerMsgHandler
{
public:
    CInnerMsgHandler() throw();
    ~CInnerMsgHandler();
    void SendStopNetworkThread();
};

#endif // DNF_INNER_MSG_HANDLER_H_
