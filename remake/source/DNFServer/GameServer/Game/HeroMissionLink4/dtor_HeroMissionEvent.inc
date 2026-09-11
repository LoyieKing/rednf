// ---------------------------------------------------------------------------
// ~HeroMissionEvent（ORIG D1/D2 @0x081665a2 同体、D0 @0x0816662c）。
// 类现为真实继承（Singleton+CEventBase），虚析构由编译器自动发射
// D1/D2（同体）+ D0（call D1 + operator delete），与 ORIG 三形态对应。
// ORIG 析构顺序：m_values(+0x24) → m_conditions(+0x0c)，与本类成员声明逆序
// 一致（编译器按声明逆序自动析构）。
// ---------------------------------------------------------------------------
HeroMissionEvent::~HeroMissionEvent()
{
    // 成员（std::map/vector）由编译器按逆序自动析构。
}