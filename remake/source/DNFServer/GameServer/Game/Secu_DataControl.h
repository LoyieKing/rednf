#ifndef GAME_SECU_DATACONTROL_H_
#define GAME_SECU_DATACONTROL_H_

// df_game_r 还原 —— Secu_DataControl（InterWave1 批唯一声明点）。
// GlobalData.cpp 持全局 s_pSecuDataControl（B 0x0941f7c8，GlobalData.h 已 extern）。
// 方法为 ORIG 真实符号；定义由 Main 统一分派属主批次，U 引用合规（§8.1）。

class CUser;

class Secu_DataControl
{
public:
    Secu_DataControl();
    void UpdateSecurityServiceState(CUser* user, unsigned char service,
                                    unsigned char flag, char* msg);   // ORIG T 0x08286e40
    void SetControlData(CUser* user, unsigned char type,
                        unsigned short v1, unsigned short v2);   // ORIG T 0x08286a30
};

#endif  // GAME_SECU_DATACONTROL_H_
