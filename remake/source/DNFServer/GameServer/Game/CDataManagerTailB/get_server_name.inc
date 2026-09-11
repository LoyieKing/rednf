// ---------------------------------------------------------------------------
// get_server_name(int) const —— ORIG T 0x08365f00
//   边界外或未知 idx 返回 "ERROR_SERVER"；服务器名字面量取自 .rodata
//   （EUC-KR 字节序列，以 \x 转义保真，提取地址见 case 注释）。
// ---------------------------------------------------------------------------
const char* CDataManager::get_server_name(int idx) const
{
    // ORIG 0x08365f03/07：`cmpl $0,idx; js`（idx<0）+ `cmpl $0x12,idx; jle`
    // 显式越界守卫 → ERROR_SERVER。跳表按 idx 直索引（0x8c332f8 表），
    // case 值 1..0x12（Ghidra C + 表[0]=default 双实证：idx=0 → ERROR_SERVER）；
    // case 0x64 落 default（ORIG 表 0..0x64 共 101 项、cmpl $0x64 上界，
    // 项 19..99 与 0、100 均填 default 块 0x8365fb9）。
    if (idx < 0 || idx > 0x12)
        return "ERROR_SERVER";
    switch (idx)
    {
    case 1:  return "\xc4\xab\xc0\xce";                      // 카인      8c3327b
    case 2:  return "\xb5\xf0\xb7\xb9\xc1\xf6\xbf\xa1";      // 디레지에  8c33280
    case 3:  return "\xbd\xc3\xb7\xce\xc4\xda";              // 시로코    8c33289
    case 4:  return "\xc7\xc1\xb7\xb9\xc0\xcc";              // 프레이    8c33290
    case 5:  return "\xc4\xab\xbd\xc3\xbe\xdf\xbd\xba";      // 카시야스  8c33297
    case 6:  return "\xc8\xfa\xb4\xf5";                      // 힐더      8c332a0
    case 7:  return "\xb7\xe7\xc5\xa9";                      // 루크      8c332a5
    case 8:  return "\xbc\xbc\xb8\xae\xbe\xc6";              // 세리아    8c332aa
    case 9:  return "\xbe\xc8\xc5\xe6";                      // 안톤      8c332b1
    case 10: return "\xb7\xcf\xbd\xc3";                      // 록시      8c332b6
    case 11: return "\xb9\xd9\xc4\xae";                      // 바칼      8c332bb
    case 12: return "\xbf\xc0\xc1\xee\xb8\xb6";              // 오즈마    8c332c0
    case 13: return "\xc4\xab\xc0\xdc";                      // 카잔      8c332c7
    case 14: return "\xc0\xcc\xba\xa5\xc6\xae""1";           // 이벤트1   8c332cc
    case 15: return "\xc0\xcc\xba\xa5\xc6\xae""2";           // 이벤트2   8c332d4
    case 16: return "\xc0\xcc\xba\xa5\xc6\xae""3";           // 이벤트3   8c332dc
    case 17: return "\xbd\xba\xc5\xb8\xc6\xae";              // 스타트    8c332e4
    case 18: return "\xc6\xdb\xbc\xb7""2";                   // 퍼섭2     8c332f1
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47:
    case 48:
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
    case 58:
    case 59:
    case 60:
    case 61:
    case 62:
    case 63:
    case 64:
    case 65:
    case 66:
    case 67:
    case 68:
    case 69:
    case 70:
    case 71:
    case 72:
    case 73:
    case 74:
    case 75:
    case 76:
    case 77:
    case 78:
    case 79:
    case 80:
    case 81:
    case 82:
    case 83:
    case 84:
    case 85:
    case 86:
    case 87:
    case 88:
    case 89:
    case 90:
    case 91:
    case 92:
    case 93:
    case 94:
    case 95:
    case 96:
    case 97:
    case 98:
    case 99:
    case 0x64:            // ORIG 跳表覆盖 0..0x64（cmpl $0x64 上界 + 101 项表，
                          // 项 19..0x64 全填 default 块 0x8365fb9；独立 return 块
                          // 防 4.4 将空 case 折叠回 default 致跳表收缩）
        return "ERROR_SERVER";
    default:
        break;
    }
    return "ERROR_SERVER";
}