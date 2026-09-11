// ORIG 0x08190eae（静态）：SigAradAvatarEnduranceSave ctor（清 +0/+4）后
// 写 uiId/endurance 两字段，走引用重载。
void AvatarRechargeServer::SaveAvatarEnduranceUnit(int key, int value)
{
    arad::SigAradAvatarEnduranceSave save;   // ORIG ctor：清 +0/+4
    save.uiId = (unsigned int)key;
    save.endurance = (unsigned int)value;
    SaveAvatarEnduranceUnit(save);
}