// df_manager_r — main（ORIG DNFManagerServerMain.cpp）
#include "DNFApplication.h"

int main(int argc, char** argv)
{
    CApplication app;
    try
    {
        app.Create(argc, argv);
        app.Process();
        app.Clear();
    }
    catch (...)
    {
        app.Clear();
    }
    return 1;
}
