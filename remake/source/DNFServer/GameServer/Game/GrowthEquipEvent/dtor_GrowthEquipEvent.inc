// ============================================================================
// 生命周期（vtable 全覆盖，保证本 TU 发出的 _ZTV16GrowthEquipEvent 可解析）
// ============================================================================
GrowthEquipEvent::~GrowthEquipEvent()
{
    // ORIG D1：恢复本类 vptr 后调 CEventBase::~CEventBase()。
}