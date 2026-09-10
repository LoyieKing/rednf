void AvatarRechargeServer::SaveAvatarEnduranceUnit(int key, int value)
{
    arad::SigAradAvatarEnduranceSave save;   // ORIG ctor：清 +0/+4
    save.uiId = (unsigned int)key;
    save.endurance = (unsigned int)value;
    SaveAvatarEnduranceUnit(save);
