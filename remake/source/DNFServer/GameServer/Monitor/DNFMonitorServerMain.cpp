// df_monitor_r — main
#include "DNFMonitorServerMain.h"

int main(int argc, char** argv)
{
    CAppBase* app = CApplicationInstance();
    app->Create(argc, argv);
    void (**vtab)(void*) = (void(**)(void*))(*(void**)app);
    vtab[2](app);
    app->Clear();
    return 1;
}
