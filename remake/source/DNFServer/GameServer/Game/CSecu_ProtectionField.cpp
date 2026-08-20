// df_game_r Game/ CSecu_ProtectionField 实现（独立 TU，从 GameStubs.cpp 拆出）。
#include "CInventory.h"

int CSecu_ProtectionField::Check(CSecu_ProtectionField* self, CUser* user, SECURITY_PROTCTION type)
{
    if (user == 0) return 1;
    unsigned int protType = self->GetProtectionType(type);
    if (protType == 0) return 0;
    int ret = self->CheckRestoreType(user, protType);
    if (ret != 0) return ret;
    ret = self->CheckTradingType(user, protType, type);
    if (ret != 0) return ret;
    ret = self->CheckEtcType(user, type);
    if (ret != 0) return ret;
    return 0;
}

int CSecu_ProtectionField::GetOppositeErr(int err)
{
    if (err == 0x88) return 0xec;
    if (err < 0x89) {
        if (err == 0x7a) return 0xee;
        if (err == 0x7b) return 0x7c;
        if (err == 0x72) return 0x73;
    } else {
        if (err == 0x89) return 0x8a;
        if ((unsigned int)(err - 0xe4) < 2) return 0x73;
    }
    return 1;
}

unsigned int CSecu_ProtectionField::GetProtectionType(SECURITY_PROTCTION type)
{
    return 0;
}

int CSecu_ProtectionField::CheckRestoreType(CUser* user, unsigned int protType)
{
    return 0;
}

int CSecu_ProtectionField::CheckTradingType(CUser* user, unsigned int protType,
                                             SECURITY_PROTCTION type)
{
    return 0;
}

int CSecu_ProtectionField::CheckEtcType(CUser* user, SECURITY_PROTCTION type)
{
    if (user == 0) return 1;
    if (user->isRestingUser()) return 0xcf;
    if ((type == (SECURITY_PROTCTION)8 || type == (SECURITY_PROTCTION)9) &&
        *(unsigned char*)((char*)user + 0x8e97d) == 0) return 0xe9;
    return 0;
}
