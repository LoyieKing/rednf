// ORIG W 0x084eb100：movl $0,(%esp); call time@plt; mov %eax,0x943dd88
void Inter_MonitorGuildPointDel::setLastDeleteTime()
{
    g_lastDeleteTime = time(0);
}