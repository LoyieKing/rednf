// ORIG D1 0x08164f3a：写回基类 vptr 后析构内嵌 condition；D0 0x08164f6e 为
// deleting 形态。虚析构定义于本 TU 保证唯一 vtable/typeinfo 输出。
BaseHeroMissionCondition::~BaseHeroMissionCondition()
{
}