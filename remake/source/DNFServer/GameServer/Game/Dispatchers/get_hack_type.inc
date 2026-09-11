// ORIG 0x08264364（报告 Dispatcher_ValidateScriptHash/get_hack_type.md）[随行依赖]：
// switch(1..5) → 0x344..0x348；0 与越界分支 ORIG 未初始化 local_8（忠实保留）。
int Dispatcher_ValidateScriptHash::get_hack_type(char type)
{
    int hackType;  // ORIG local_8：无 default 分支时不写入
    switch (type)
    {
    case 1: hackType = 0x344; break;
    case 2: hackType = 0x345; break;
    case 3: hackType = 0x346; break;
    case 4: hackType = 0x347; break;
    case 5: hackType = 0x348; break;
    }
    return hackType;
}