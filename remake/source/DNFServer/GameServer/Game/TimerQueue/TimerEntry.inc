TimerEntry::TimerEntry() throw()
{
    // ORIG _ZN10TimerEntryC1Ev 0x847e85c：objType=0 / param1=0 / msg=0x75(DEFAULT) /
    // time=0 / param2=0 / param3=0，逐字段 movl 清零。
    objType = OBJ_TYPE_0;
    param1 = 0;
    msg = TIMER_MESSAGE_DEFAULT;
    time = 0;
    param2 = 0;
    param3 = 0;
