stStepRewardStackableItemScript::stStepRewardStackableItemScript()
{
    clear();   // ORIG C1 0x83714c6 调本类 clear()（W 0x08371508），而非内联 map::clear
}