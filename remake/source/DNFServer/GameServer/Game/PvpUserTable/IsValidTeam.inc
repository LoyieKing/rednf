bool PvpUserTable::IsValidTeam(int team)
{
    // ORIG 0x085d4a14：`cmpl $0; js 岛; cmpl $7; jg 岛; mov $1; jmp; mov $0`
    // （返回类型按 caller 0x085d5081 的 xor$1+testb 形定为 bool，见 PvPTypes.h）
    return (team >= 0 && team <= 7);
