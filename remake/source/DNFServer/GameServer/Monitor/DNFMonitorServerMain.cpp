// df_monitor_r — main
#include "DNFMonitorServerMain.h"

int main(int argc, char** argv)
{
    CApplication* app = (CApplication*)CApplicationInstance();
    app->Create(argc, argv);
    app->Process();
    app->Clear();
    return 1;
}
