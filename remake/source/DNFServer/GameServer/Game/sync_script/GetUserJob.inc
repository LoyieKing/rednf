const char* CSyncScript::GetUserJob(ENUM_CHARACTERJOB job)
{
    // ORIG 0x0861d2dc：switch 跳表（cmp $7; ja 边界 + .rodata 8 项表）→
    // EUC-KR 职业名常量（.rodata 0x08ce2e6c.. objdump 实证，与 GetJobString 同
    // 编码），default 为单引号串（0x08ce2e28 = "'"）。逐 case 独立 return。
    switch ((int)job)
    {
    case 0:  return "\xb1\xcd\xb0\xcb\xbb\xe7";                // 귀검사
    case 1:  return "\xb0\xdd\xc5\xf5\xb0\xa1";                // 격투가
    case 2:  return "\xb0\xc5\xb3\xca";                        // 거너
    case 3:  return "\xb8\xb6\xb9\xfd\xbb\xe7";                // 마법사
    case 4:  return "\xc7\xc1\xb8\xae\xbd\xba\xc6\xae";        // 프리스트
    case 5:  return "\xbf\xa9\xb0\xc5\xb3\xca";                // 여거너
    case 6:  return "\xb5\xb5\xc0\xfb";                        // 도적
    case 7:  return "\xb3\xb2\xb0\xdd\xc5\xf5\xb0\xa1";        // 남격투가
    default: return "'";
    }
}