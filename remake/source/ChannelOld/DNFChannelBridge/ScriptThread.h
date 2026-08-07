#ifndef SCRIPTTHREAD_H_
#define SCRIPTTHREAD_H_

#include "ChannelServiceApp.h"

namespace ChannelServiceApp
{
    class ScriptThread : public Thread, public TManager<ChannelService>
    {
    public:
        ScriptThread();
        ~ScriptThread();
        virtual void loop(void* temp);
    };
}

#endif // SCRIPTTHREAD_H_
