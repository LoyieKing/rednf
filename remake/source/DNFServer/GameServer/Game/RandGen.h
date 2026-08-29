// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + docs/class_func_reports/RandGen/（getFloat01 0x08ae0206/0x08ae0244、getFloat 0x08ae0288/0x08ae02a4，T 强符号）。
#ifndef DNF_GAME_RANDGEN_H_
#define DNF_GAME_RANDGEN_H_

class XorRand4;  // 前置声明：GlobalData.cpp 引用，权威头待建

// 4 个方法均为静态（getFloat01() 无参调用 XorRand4::getStaticUInt32()）。
class RandGen
{
public:
    static float getFloat01();                       // T 0x08ae0206；返回 float【推断】
    static float getFloat01(XorRand4*);              // T 0x08ae0244；返回 float【推断】
    static float getFloat(float, float);             // T 0x08ae0288；返回 float【推断】
    static float getFloat(XorRand4*, float, float);  // T 0x08ae02a4；返回 float【推断】
};

#endif  // DNF_GAME_RANDGEN_H_
