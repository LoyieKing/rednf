// df_statics_r — DNFStatisticServerMain（ORIG DNFStatisticServerMain.o 拆分）
#include <stdio.h>

#include "DNFApplication.h"

int main(int argc, char** argv)
{
    CAppBase* app = CApplicationInstance();
    try
    {
        app->Create(argc, argv);
        app->Process();
        app->Clear();
    }
    catch (...)
    {
        app->Clear();
    }
    return 1;
}
