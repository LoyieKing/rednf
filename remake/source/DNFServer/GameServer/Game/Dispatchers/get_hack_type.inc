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
