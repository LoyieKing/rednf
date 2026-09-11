// ORIG guard 单例（guard var 0x943c070 / instance 0x943c080）：__cxa_guard
// 双检锁 + atexit 注册。
user_creature::CCreatureItemFactory* user_creature::GetInstanceCreatureItemFactory()
{
    static user_creature::CCreatureItemFactory instance;
    return &instance;
}
