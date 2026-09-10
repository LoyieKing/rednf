stQuestTriggerState_t::stQuestTriggerState_t()
{
    // ORIG：movl $0x0,(%eax); movl $0x0,0x4(%eax) —— 按 8 字节（2×int）整块清零
    *(long long*)this = 0;
