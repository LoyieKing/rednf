#ifndef GAME_DNFFLIB_H_
#define GAME_DNFFLIB_H_

// HdrCreate2 批（/tmp/missing_h_chunk_ab）：ORIG 有导出符号但仓库无 .h 声明的类。
// 仅声明骨架（不入 CMake，无 cpp 实现）。参数类型按 ORIG demangled 原样；
// 返回类型与 static/成员属性未逐项核实者标【推断】，实现时须以 docs/class_func_reports 为准。

class DNFFLib
{
public:
    // ORIG 0x8108873 _ZN7DNFFLib10Binary2HexEPKhiPc (size 0x0000005c)  报告: DNFFLib/Binary2Hex.md  【推断:static?】
    int Binary2Hex(unsigned char const*, int, char*);   // 返回类型【推断】
    // ORIG 0x81088cf _ZN7DNFFLib10Hex2BinaryEPKcPhi (size 0x00000058)  报告: DNFFLib/Hex2Binary.md  【推断:static?】
    int Hex2Binary(char const*, unsigned char*, int);   // 返回类型【推断】
    // ORIG 0x8108b06 _ZN7DNFFLib12get_rand_intEi (size 0x000000f5)  报告: DNFFLib/get_rand_int.md  【推断:static?】
    int get_rand_int(int);   // 返回类型【推断】
    // ORIG 0x81089c8 _ZN7DNFFLib13ExplodeStringEPcS0_PS0_i (size 0x000000d0)  报告: DNFFLib/ExplodeString.md  【推断:static?】
    int ExplodeString(char*, char*, char**, int);   // 返回类型【推断】
    // ORIG 0x8108927 _ZN7DNFFLib13PrintTextFileEPcS0_ (size 0x00000041)  报告: DNFFLib/PrintTextFile.md  【推断:static?】
    int PrintTextFile(char*, char*);   // 返回类型【推断】
    // ORIG 0x8108968 _ZN7DNFFLib14fPrintTextFileEPcS0_z (size 0x00000060)  报告: DNFFLib/fPrintTextFile.md  【推断:static?】
    int fPrintTextFile(char*, char*, ...);   // 返回类型【推断】
    // ORIG 0x8108bfb _ZN7DNFFLib15CharacSetSwitchEPKcS1_PcS2_ (size 0x00000113)  报告: DNFFLib/CharacSetSwitch.md  【推断:static?】
    int CharacSetSwitch(char const*, char const*, char*, char*);   // 返回类型【推断】
    // ORIG 0x8108d0e _ZN7DNFFLib16ConvertGBKtoUTF8EPcS0_ (size 0x0000003d)  报告: DNFFLib/ConvertGBKtoUTF8.md  【推断:static?】
    int ConvertGBKtoUTF8(char*, char*);   // 返回类型【推断】
    // ORIG 0x8108d4b _ZN7DNFFLib16ConvertUTF8toGBKEPcS0_ (size 0x0000003d)  报告: DNFFLib/ConvertUTF8toGBK.md  【推断:static?】
    int ConvertUTF8toGBK(char*, char*);   // 返回类型【推断】
    // ORIG 0x8108ad7 _ZN7DNFFLib8Make_DirEPc (size 0x0000002f)  报告: DNFFLib/Make_Dir.md  【推断:static?】
    int Make_Dir(char*);   // 返回类型【推断】
    // ORIG 0x8108a98 _ZN7DNFFLib9Sleep_ExtEii (size 0x0000003f)  报告: DNFFLib/Sleep_Ext.md  【推断:static?】
    int Sleep_Ext(int, int);   // 返回类型【推断】
};

#endif  // GAME_DNFFLIB_H_
