bool checkValiedJobName(const char* name)   // [b28d-Main·P292] int→bool
{
    static const char* const kJobTable[12] = {
        "[swordman]",        // 0
        "[fighter]",         // 1
        "[gunner]",          // 2
        "[mage]",            // 3
        "[priest]",          // 4
        "[at gunner]",       // 5
        "[thief]",           // 6
        "[at fighter]",      // 7
        "[at mage]",         // 8
        "[demonic swordman]",// 9
        "[creator mage]",    // 10
        "[all]"              // 11
    };
    for (int i = 0; i <= 0xb; ++i)
    {
        if (strcmp(name, kJobTable[i]) == 0)
        {
            return true;
        }
    }
    return false;
