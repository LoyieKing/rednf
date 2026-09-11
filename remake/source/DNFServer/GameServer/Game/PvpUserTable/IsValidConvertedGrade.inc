// b28d-54 判词：ORIG 0x085d4a32（GetExpectVal 两处调用点 0x085d4b4a/0x085d4b5f 均
//   `call; xor $1,%eax; test %al,%al; je`）⇒ 返回 bool（P248 档①）。
bool PvpUserTable::IsValidConvertedGrade(int grade)
{
    return (grade >= 0 && grade <= 0x14) ? 1 : 0;
}