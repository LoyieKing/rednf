// ORIG T 0x089bcfed（switch 跳表 0x8e19c34）：
// 2→0、6→1、7→2、9→3、11→4、其余 -1。
int getActiveStatusDamageIndex(ENUM_ACTIVESTATUS status)
{
    int result = -1;
    switch (static_cast<int>(status))
    {
    case 2:  result = 0; break;
    case 6:  result = 1; break;
    case 7:  result = 2; break;
    case 9:  result = 3; break;
    case 11: result = 4; break;
    }
    return result;
}