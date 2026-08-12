// df_monitor_r — main
#include "DNFMonitorServerMain.h"

int main(int argc, char** argv)
{
    register CAppBase* app = CApplicationInstance();
    app->Create(argc, argv);
    app->Process();
    app->Clear();
    return 1;
}
