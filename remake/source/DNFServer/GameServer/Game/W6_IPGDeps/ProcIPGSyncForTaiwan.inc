// ---------------------------------------------------------------------------
// ProcIPGSyncForTaiwan —— ORIG T 0x081048c6 (0x992)。switch(data->m_type)：
// 0=charge、2=purchase、3=cancel、4=getBalance、5=confirm、6=ceraPoint、
// 其余（含 1）记日志。cash 取 m_mgr->m_billingCash（ORIG 6 处均直接解引用
// m_mgr(+0xc)→+0xec，无 getBillingCash call）。载荷按命令族视图访问
// （m_cmd.m_input / m_output / m_cancel / m_point / m_query，见 TaiwanCash.h）。
// §6.2 巨件核销（b25）：P167 修前 50/56 [多6 getBillingCash]→修后 50/50
// [0缺0多]；gap 0→0（orig 601 / our 601 指令数相等）；修法：6 处
// getBillingCash() 调用改 m_mgr->m_billingCash 直接成员访问（ORIG 本函数
// 内联解引用 +0xec，访问器不产生 call；TaiwanCash.cpp 的 out-of-line 定义
// 保留供 0x08175f94 等真实 call 点使用）。残差为 N82 槽序/ndiff 951 字节层
// 排布（详见 docs/asm_translation_patterns.md 负例表）。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_IPGDeps] verdict=DIFF/N4+N7+N107 封顶（§6.2 三件套）。
// 本轮修法（四组，全部有 ORIG 证据）：
//  ① **case 块物理顺序重排**：ORIG jump table @0x8b42290 七项目标地址排序 =
//     case 4/2/5/3/0/6/1，P46「叶块排放顺序 = case 书写顺序」⇒ 源码按 4,2,5,3,0,6 书写；
//  ② **删 11 个视图指针局部**（rq/in/cash×2/out/ro/cv×2/pt/rv，P183/P134）——
//     ORIG 各点 `mov 0xc(%ebp),%eax; add $0xNN` 重算，指针局部使帧 0xdc（ORIG 0x9c）；
//  ③ **case 2 pay/price 内联实参**（P0：ORIG 两 atoi 结果只在 %ebx/%eax 跨第二 atoi
//     保活、无槽往返 ⇒ 原码非具名局部）；
//  ④ **case 3/5 tran 改整体赋值** `tran = data->m_cmd.m_cancel.m_tran;`（P247：ORIG
//     单基址 0x1f0/0x1f4/0x1f8 三 dword 连续拷贝）。
// 三件套读数（447 与 446 双后端一致）：① gap=16（601 vs 585，逐点 = 下述 add-vs-lea
// 每处少 1 条）；② p167 调用点差集 CLEAR（50/50，0 缺 0 多，/tmp/tu_W6_IPGDeps.o
// 复验）；③ 帧 0x9c 与 ORIG 全等、槽集差 1（多 0x68 = case 0 tran 与 case 6 缓存
// 复用边界的 4B 槽），语义逐块比对完成（Ghidra C 段 622-812 逐 case 核对）。
// 残差 = 4.1 `add $K,%eax; mov %eax,%reg` vs 4.4 `lea K(%eax),%reg`（N4/P30/N107，
// 探针 /tmp/addprobe{,2,3}.cpp 证实 4.4 在"地址+另一变参并存"时恒 lea）
// + 间接调用/实参寄存器角色（N7）。语义完成、byte 层 N 封顶。
// [S2W6Deps 复核] 六处 LogManager::logFormat 实参逐参核对**一致**（ORIG 0x08104947
// `$"IPG_QUERY user: %s reason: %d"` / 0x08104957 `$&__PRETTY_FUNCTION__` /
// 0x0810495f `$"IPGMgr.cpp"`；余 5 处同形）——报告里我方 `$0x1e/$0x440/$0x0`
// 是 .o 未链接的 .rodata 段内偏移，**非立即数实参**（§6.5 串盲区已排除；
// strall.py 四 TU 串审计：仅 _GLOBAL__I/vtable 假阳性，无语义串缺/多）。
SIPGData* CIPGMgr::CIPGWorker::ProcIPGSyncForTaiwan(SIPGData* data)
{
    SIPGData* result = 0;

    switch (data->m_type)
    {
    // [b28d-solo-W6_IPGDeps] case 块物理排布：ORIG jump table @0x8b42290 七项
    // 目标地址排序 = case 4(0x81048f1)、2(0x8104ad1)、5(0x8104d62)、3(0x8104da0)、
    // 0(0x8104dde)、6(0x8104f52)、1/default(0x8105211)；P46「叶块排放顺序 = case
    // 书写顺序」⇒ 源码 case 必须按此序书写（纯 jump table，无决策树内联风险）。
    case 4:  // IPG_TYPE_QUERY：getBalance(account, &bal, &point)
    {
        int bal = 0;    // -0x34
        int point = 0;  // -0x30
        int rc = m_mgr->m_billingCash->getBalance(data->m_accName, bal, point);
        if (rc != 0)
        {
            LogManager::logFormat(1, "IPGMgr.cpp", __PRETTY_FUNCTION__, 0x243,
                                  "IPG_QUERY user: %s reason: %d",
                                  data->m_account, rc);
            break;
        }
        result = m_mgr->AllocIPGData();
        result->m_recordLen = data->m_recordLen;
        strncpy(result->m_envCode, data->m_envCode, 0x14);
        result->m_productCode = data->m_productCode;
        result->m_type = (IPG_DATA_TYPE)4;
        result->m_notifyFlag = data->m_notifyFlag;
        memset(&result->m_account, 0, 0x99);
        strncpy(result->m_account, data->m_account, 0x14);
        strncpy(result->m_accName, data->m_accName, 0x28);
        strncpy(result->m_gameCode, data->m_gameCode, 5);
        // [b28d-solo-W6_IPGDeps] 删 rq 具名指针局部：ORIG 0x8104a.. 各 sprintf 目标
        // 逐点重算 `mov 0xc(%ebp),%eax; add $0xNN`（P183/P134），指针局部使 4.4 帧
        // 多 4B 且槽序整体深移。
        sprintf(result->m_cmd.m_query.m_ceraValues[0], "%d", point);  // asm 写序：+0x69←-0x30(point)、+0xb5←-0x34(bal)、+0xb1←rc
        sprintf(result->m_cmd.m_query.m_ceraPoint, "%d", bal);
        sprintf(result->m_cmd.m_query.m_retCode, "%d", rc);
        result->m_callback = data->m_callback;
        break;
    }

    case 2:  // IPG_TYPE_OUTPUT：purchase（ORIG 实参：account=data->m_accName(+0x3c)、
             // 第二字符串=m_output.m_recvCharac(+0x1e8)，均照 asm）
    {
        Taiwan::BillTransaction tran;          // -0x40..-0x38（本栈帧局部）
        // [b28d-solo-W6_IPGDeps] 删 out/cash 具名局部：ORIG 0x8104ad1 段对每个
        // 实参重算 `mov 0xc(%ebp),%eax; add $0xNN`（P183），指针局部使帧多 8B。
        // [b28d-solo-W6_IPGDeps] P0：ORIG 0x8104ae3/0x8104af3 两 atoi 结果只在
        // %ebx/%eax 跨第二 atoi 保活、无槽往返 ⇒ 原码是内联实参（非 pay/price 具名局部）。
        int rc = m_mgr->m_billingCash->purchase(0ULL,
                                                atoi(data->m_cmd.m_output.m_price),
                                                atoi(data->m_cmd.m_output.m_pay),
                                                data->m_accName,
                                                data->m_cmd.m_output.m_recvCharac,
                                                tran);
        if (rc != 0)
        {
            LogManager::logFormat(1, "IPGMgr.cpp", __PRETTY_FUNCTION__, 0x269,
                                  "IPG_OUTPUT user: %s, charac: %s reason: %d",
                                  data->m_account, data->m_cmd.m_output.m_recvCharac, rc);
            break;
        }
        result = m_mgr->AllocIPGData();
        result->m_recordLen = data->m_recordLen;
        strncpy(result->m_envCode, data->m_envCode, 0x14);
        result->m_productCode = data->m_productCode;
        result->m_type = (IPG_DATA_TYPE)2;                           // 响应类型
        result->m_notifyFlag = data->m_notifyFlag;
        memset(&result->m_account, 0, 0x1fc);                        // 载荷前 0x1fc 字节
        strcpy(result->m_account, data->m_account);
        strcpy(result->m_accName, data->m_accName);
        strcpy(result->m_gameCode, data->m_gameCode);
        strcpy(result->m_cmd.m_output.m_price, data->m_cmd.m_output.m_price);
        strcpy(result->m_cmd.m_output.m_pay, data->m_cmd.m_output.m_pay);
        strcpy(result->m_cmd.m_output.m_arg7D, data->m_cmd.m_output.m_arg7D);
        strcpy(result->m_cmd.m_output.m_desc91, data->m_cmd.m_output.m_desc91);
        strcpy(result->m_cmd.m_output.m_descF5, data->m_cmd.m_output.m_descF5);
        result->m_cmd.m_output.m_tranField0 = tran.m_field0;         // 08104cf4 `mov %edx,0x1dc(%eax)`
        result->m_cmd.m_output.m_tranNo = tran.m_tranNo;             // 08104cfd/08104d06 两 dword
        sprintf(result->m_cmd.m_output.m_resultCode, "%d", rc);
        memcpy(result->m_cmd.m_output.m_message, "", 1);             // 08104d46 DAT_08b421df = "\0"（n=1）
        result->m_callback = data->m_callback;
        break;
    }

    case 5:  // IPG_TYPE_OUTPUT_CONFIRM：BillTransaction@0x1f0 → confirm
    {
        Taiwan::BillTransaction tran;
        // [b28d-solo-W6_IPGDeps] 删 cv 具名指针局部：ORIG 0x8104d62/0x8104da0 段
        // 直接 `mov 0xc(%ebp),%eax; mov 0x1f0/0x1f4/0x1f8(%eax),%edx` 单基址
        // 三 dword 连续拷贝（低→高）⇒ 原码是 BillTransaction 整体赋值（P247）。
        tran = data->m_cmd.m_cancel.m_tran;
        m_mgr->m_billingCash->confirm(tran);
        break;
    }

    case 3:  // IPG_TYPE_OUTPUT_CANCEL：BillTransaction@0x1f0 → cancel
    {
        Taiwan::BillTransaction tran;
        tran = data->m_cmd.m_cancel.m_tran;
        m_mgr->m_billingCash->cancel(tran);
        break;
    }

    case 0:  // IPG_TYPE_INPUT：charge → confirm
    {
        Taiwan::BillTransaction tran;   // -0x68..-0x5c
        // [b28d-solo-W6_IPGDeps] 删 in/cash 具名局部：ORIG 0x8104dde 段对每实参
        // 重算 `mov 0xc(%ebp),%eax` + add/偏移（P183），局部使帧多 8B 且槽深移。
        int rc = m_mgr->m_billingCash->charge(data->m_account,
                                              data->m_cmd.m_input.m_serviceType,
                                              0,
                                              data->m_cmd.m_input.m_price,
                                              "",
                                              "",
                                              "",
                                              data->m_characName,
                                              0ULL,
                                              tran);
        if (rc != 0)
        {
            LogManager::logFormat(1, "IPGMgr.cpp", __PRETTY_FUNCTION__, 0x2a7,
                                  "IPG_INPUT charge user: %s, charac: %s reason: %d",
                                  data->m_account, data->m_characName, rc);
            break;
        }
        rc = m_mgr->m_billingCash->confirm(tran);
        if (rc != 0)
        {
            char* tranStr = NumberToString(tran.m_tranNo, 0);   // tran+0x04..0x0b
            LogManager::logFormat(1, "IPGMgr.cpp", __PRETTY_FUNCTION__, 0x2af,
                                  "IPG_INPUT confirm user: %s, charac: %s reason: %d tran :%s",
                                  data->m_account, data->m_characName, rc, tranStr);
        }
        break;
    }

    case 6:  // IPG_TYPE_POINT：ceraPoint；A 型命令位不回包
    {
        // [b28d-solo-W6_IPGDeps] 删 cash/pt/rv 具名局部（P183：ORIG 各点重算）。
        int rc = m_mgr->m_billingCash->ceraPoint(data->m_accName,
                                                 data->m_gameCode,
                                                 (char)data->m_cmd.m_point.m_command,
                                                 data->m_cmd.m_point.m_itemNo,
                                                 data->m_cmd.m_point.m_arg94,
                                                 (char)data->m_cmd.m_point.m_cmdA,
                                                 (char)data->m_cmd.m_point.m_cmdB,
                                                 (int)data->m_cmd.m_point.m_point);
        if (rc != 0)
        {
            sprintf(data->m_cmd.m_point.m_reasonB3, "%d", rc);
            LogManager::logFormat(1, "IPGMgr.cpp", __PRETTY_FUNCTION__, 0x2c4,
                                  "IPG_CERA_POINT user: %s reason: %d",
                                  data->m_accName, rc);
            break;
        }
        if (data->m_cmd.m_point.m_command == 0x41)
        {
            break;   // 'A' 命令位：无响应数据
        }
        result = m_mgr->AllocIPGData();
        result->m_recordLen = data->m_recordLen;
        strncpy(result->m_envCode, data->m_envCode, 0x14);
        result->m_productCode = data->m_productCode;
        data->m_type = (IPG_DATA_TYPE)2;  // ORIG 081050b9：请求数据 type 同步改 2
        result->m_type = (IPG_DATA_TYPE)2;
        result->m_notifyFlag = data->m_notifyFlag;
        memset(&result->m_account, 0, 0x1fc);
        strncpy(result->m_account, "", 0x14);
        strncpy(result->m_gameCode, "", 5);
        strncpy(result->m_cmd.m_output.m_message, "cera point", 100);
        strncpy(result->m_accName, data->m_accName, 0x28);
        strncpy(result->m_cmd.m_output.m_price, data->m_cmd.m_point.m_itemNo, 8);
        strncpy(result->m_cmd.m_output.m_arg7D, data->m_cmd.m_point.m_arg94, 0x14);
        sprintf(result->m_cmd.m_output.m_resultCode, "%d", rc);
        sprintf(result->m_cmd.m_output.m_pay, "%d", data->m_cmd.m_point.m_point);
        result->m_callback = data->m_callback;
        break;
    }

    default:
        LogManager::logFormat(1, "IPGMgr.cpp", __PRETTY_FUNCTION__, 0x2e9,
                              "IPGDataType : %d", data->m_type);
        break;
    }

    return result;
}
