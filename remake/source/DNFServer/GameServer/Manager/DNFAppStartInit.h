#ifndef DNF_APP_START_INIT_H_
#define DNF_APP_START_INIT_H_

#include <string>

class CApplication;

// ---- CAppInit ----
class CAppInit
{
public:
    CAppInit();
    virtual ~CAppInit();
    virtual void Init(CApplication* app, int argc, char** argv) {}
};

// ---- CAppStartInit ----
class CAppStartInit : public CAppInit
{
public:
    CAppStartInit();
    virtual ~CAppStartInit();
    virtual void Init(CApplication* app, int argc, char** argv);
    int Init_Daemon(int argc, char** argv);
    int Save_pid(const std::string& path);
};

#endif  // DNF_APP_START_INIT_H_
