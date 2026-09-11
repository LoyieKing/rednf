void SDCActiveStatusSyncCommon::makeEtcParameters(ENUM_ACTIVESTATUS status,
                                                  int level,
                                                  std::vector<int>& out)
// 0x0808d50e：jump table（cmp $0x11; ja; mov table(,%eax,4); jmp *%eax）。
// [b28d-solo-W9_SDCCommon] ORIG 每个 push_back 是「(int) 右值实参」形：
// call → 4.4 内建 RC=truncate 序列（fnstcw/fldcw/fistpl，共享 -0x7a/-0x7c/-0x80）
// → 临时对象槽（-0x6c..-0xc 共 25 个，首次使用最深）→ lea + push_back(int&&)。
// p167 差集：MISSING push_back(int&&)×25 / EXTRA push_back(int const&)×25 ⇒ 必须
// 选右值重载（P270/P234）。具名 tv + 左值实参既选错重载，又使 4.4 对 dead float
// 返回值追加 22 条 `fstp %st(0)`（N85 伪残差，非真差）。探针 /tmp/sdc_p11.cpp b
// 与 /tmp/sdc_p12.cpp m 逐条全等（含槽分配方向）。
{
    switch (status)
    {
    case ACTIVE_STATUS_SLOW:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 1)));
        break;
    case 2:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        break;
    case 4:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 1)));
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 2)));
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 3)));
        break;
    case 5:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 1)));
        break;
    case 6:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        break;
    case 7:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        break;
    case 9:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 1)));
        out.push_back(1);
        out.push_back(3000);
        break;
    case 10:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        break;
    case 11:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        break;
    case 12:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 1)));
        break;
    case 13:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 1)));
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 2)));
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 3)));
        break;
    case 15:
        out.push_back(1);
        break;
    case ACTIVE_STATUS_17:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        break;
    default:
        break;
    }
}