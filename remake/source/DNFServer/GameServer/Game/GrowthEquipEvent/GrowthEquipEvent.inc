GrowthEquipEvent::GrowthEquipEvent()
{
    // ORIG：CEventBase() 置基类 vptr 与 stADDINFO/+0x08 flag 后写本类 vptr；
    // 无自有成员，编译器生成的基类调用 + vptr 赋值即为忠实还原。
}