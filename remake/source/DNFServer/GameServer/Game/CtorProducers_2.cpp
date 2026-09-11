// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CtorProducers_2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// CtorProducers_2.cpp — Ctor_Group2 批次：构造/装载 producer 真实实现
//
// 本 TU 提供以下 ORIG 符号（全部经 docs/class_func_reports 报告 + objdump 转写）：
//   DisjointScript         C1 0x08375d4c / clear 0x08375bca / D1 0x0837dff8
//   DisJoint               C1 0x0837e532 / LoadDisJoint T 0x0847300c
//   StrikerScript          getInsance/ctor/dtor/clear/create/destory（instance_
//                          B 0x09500978）/ importScript 系列声明见头，定义待续批
//   CharacManageScript     instance_ B 0x094f6b98 / getInsance/ctor/dtor/clear/importScript
//   SDCActiveStatusSyncCommon ctor W 0x08234870 / dtor W 0x08234876（空体）
//   SDCActiveStatusSyncServer ctor W 0x0823487c / dtor W 0x08234890 /
//                          loadActiveStatusScript T 0x0808e13c
//
// 不在本 TU 提供（避免链接重复，归属既有注册 TU）：
//   LotteryInfo C1/clear/D1 → CStackableItem.cpp 已定义（ORIG 0x083706d4/069c/4cfc）。
//
// 声明先行（定义待对应域批次，见交付报告）：
//   importDisjointScript / importCompoundAvatarTable / importActiveStatusTable /
//   ContentsType::getType / importSkillDataUp
// ==================== ANON-PAD (P203 `._381` 对齐) ====================
// [GiantB 2026-09-11] TU 匿名类型计数器只增不减；DisJoint.h 内 m_additional 的
// 自然编号实测 = 48（`_ZN14DisjointScript4._48C1Ev`），ORIG 目标 = `._381`
// ⇒ 在首个 include 前垫 333 个零符号哑匿名类型（每个占 1 编号，不发射符号）。
// 消费点 ScriptDepsA.cpp/CDataManager.cpp 各自独立计数（另测）。
typedef struct { char m_pad0; } AnonymousPad000;
typedef struct { char m_pad1; } AnonymousPad001;
typedef struct { char m_pad2; } AnonymousPad002;
typedef struct { char m_pad3; } AnonymousPad003;
typedef struct { char m_pad4; } AnonymousPad004;
typedef struct { char m_pad5; } AnonymousPad005;
typedef struct { char m_pad6; } AnonymousPad006;
typedef struct { char m_pad7; } AnonymousPad007;
typedef struct { char m_pad8; } AnonymousPad008;
typedef struct { char m_pad9; } AnonymousPad009;
typedef struct { char m_pad10; } AnonymousPad010;
typedef struct { char m_pad11; } AnonymousPad011;
typedef struct { char m_pad12; } AnonymousPad012;
typedef struct { char m_pad13; } AnonymousPad013;
typedef struct { char m_pad14; } AnonymousPad014;
typedef struct { char m_pad15; } AnonymousPad015;
typedef struct { char m_pad16; } AnonymousPad016;
typedef struct { char m_pad17; } AnonymousPad017;
typedef struct { char m_pad18; } AnonymousPad018;
typedef struct { char m_pad19; } AnonymousPad019;
typedef struct { char m_pad20; } AnonymousPad020;
typedef struct { char m_pad21; } AnonymousPad021;
typedef struct { char m_pad22; } AnonymousPad022;
typedef struct { char m_pad23; } AnonymousPad023;
typedef struct { char m_pad24; } AnonymousPad024;
typedef struct { char m_pad25; } AnonymousPad025;
typedef struct { char m_pad26; } AnonymousPad026;
typedef struct { char m_pad27; } AnonymousPad027;
typedef struct { char m_pad28; } AnonymousPad028;
typedef struct { char m_pad29; } AnonymousPad029;
typedef struct { char m_pad30; } AnonymousPad030;
typedef struct { char m_pad31; } AnonymousPad031;
typedef struct { char m_pad32; } AnonymousPad032;
typedef struct { char m_pad33; } AnonymousPad033;
typedef struct { char m_pad34; } AnonymousPad034;
typedef struct { char m_pad35; } AnonymousPad035;
typedef struct { char m_pad36; } AnonymousPad036;
typedef struct { char m_pad37; } AnonymousPad037;
typedef struct { char m_pad38; } AnonymousPad038;
typedef struct { char m_pad39; } AnonymousPad039;
typedef struct { char m_pad40; } AnonymousPad040;
typedef struct { char m_pad41; } AnonymousPad041;
typedef struct { char m_pad42; } AnonymousPad042;
typedef struct { char m_pad43; } AnonymousPad043;
typedef struct { char m_pad44; } AnonymousPad044;
typedef struct { char m_pad45; } AnonymousPad045;
typedef struct { char m_pad46; } AnonymousPad046;
typedef struct { char m_pad47; } AnonymousPad047;
typedef struct { char m_pad48; } AnonymousPad048;
typedef struct { char m_pad49; } AnonymousPad049;
typedef struct { char m_pad50; } AnonymousPad050;
typedef struct { char m_pad51; } AnonymousPad051;
typedef struct { char m_pad52; } AnonymousPad052;
typedef struct { char m_pad53; } AnonymousPad053;
typedef struct { char m_pad54; } AnonymousPad054;
typedef struct { char m_pad55; } AnonymousPad055;
typedef struct { char m_pad56; } AnonymousPad056;
typedef struct { char m_pad57; } AnonymousPad057;
typedef struct { char m_pad58; } AnonymousPad058;
typedef struct { char m_pad59; } AnonymousPad059;
typedef struct { char m_pad60; } AnonymousPad060;
typedef struct { char m_pad61; } AnonymousPad061;
typedef struct { char m_pad62; } AnonymousPad062;
typedef struct { char m_pad63; } AnonymousPad063;
typedef struct { char m_pad64; } AnonymousPad064;
typedef struct { char m_pad65; } AnonymousPad065;
typedef struct { char m_pad66; } AnonymousPad066;
typedef struct { char m_pad67; } AnonymousPad067;
typedef struct { char m_pad68; } AnonymousPad068;
typedef struct { char m_pad69; } AnonymousPad069;
typedef struct { char m_pad70; } AnonymousPad070;
typedef struct { char m_pad71; } AnonymousPad071;
typedef struct { char m_pad72; } AnonymousPad072;
typedef struct { char m_pad73; } AnonymousPad073;
typedef struct { char m_pad74; } AnonymousPad074;
typedef struct { char m_pad75; } AnonymousPad075;
typedef struct { char m_pad76; } AnonymousPad076;
typedef struct { char m_pad77; } AnonymousPad077;
typedef struct { char m_pad78; } AnonymousPad078;
typedef struct { char m_pad79; } AnonymousPad079;
typedef struct { char m_pad80; } AnonymousPad080;
typedef struct { char m_pad81; } AnonymousPad081;
typedef struct { char m_pad82; } AnonymousPad082;
typedef struct { char m_pad83; } AnonymousPad083;
typedef struct { char m_pad84; } AnonymousPad084;
typedef struct { char m_pad85; } AnonymousPad085;
typedef struct { char m_pad86; } AnonymousPad086;
typedef struct { char m_pad87; } AnonymousPad087;
typedef struct { char m_pad88; } AnonymousPad088;
typedef struct { char m_pad89; } AnonymousPad089;
typedef struct { char m_pad90; } AnonymousPad090;
typedef struct { char m_pad91; } AnonymousPad091;
typedef struct { char m_pad92; } AnonymousPad092;
typedef struct { char m_pad93; } AnonymousPad093;
typedef struct { char m_pad94; } AnonymousPad094;
typedef struct { char m_pad95; } AnonymousPad095;
typedef struct { char m_pad96; } AnonymousPad096;
typedef struct { char m_pad97; } AnonymousPad097;
typedef struct { char m_pad98; } AnonymousPad098;
typedef struct { char m_pad99; } AnonymousPad099;
typedef struct { char m_pad100; } AnonymousPad100;
typedef struct { char m_pad101; } AnonymousPad101;
typedef struct { char m_pad102; } AnonymousPad102;
typedef struct { char m_pad103; } AnonymousPad103;
typedef struct { char m_pad104; } AnonymousPad104;
typedef struct { char m_pad105; } AnonymousPad105;
typedef struct { char m_pad106; } AnonymousPad106;
typedef struct { char m_pad107; } AnonymousPad107;
typedef struct { char m_pad108; } AnonymousPad108;
typedef struct { char m_pad109; } AnonymousPad109;
typedef struct { char m_pad110; } AnonymousPad110;
typedef struct { char m_pad111; } AnonymousPad111;
typedef struct { char m_pad112; } AnonymousPad112;
typedef struct { char m_pad113; } AnonymousPad113;
typedef struct { char m_pad114; } AnonymousPad114;
typedef struct { char m_pad115; } AnonymousPad115;
typedef struct { char m_pad116; } AnonymousPad116;
typedef struct { char m_pad117; } AnonymousPad117;
typedef struct { char m_pad118; } AnonymousPad118;
typedef struct { char m_pad119; } AnonymousPad119;
typedef struct { char m_pad120; } AnonymousPad120;
typedef struct { char m_pad121; } AnonymousPad121;
typedef struct { char m_pad122; } AnonymousPad122;
typedef struct { char m_pad123; } AnonymousPad123;
typedef struct { char m_pad124; } AnonymousPad124;
typedef struct { char m_pad125; } AnonymousPad125;
typedef struct { char m_pad126; } AnonymousPad126;
typedef struct { char m_pad127; } AnonymousPad127;
typedef struct { char m_pad128; } AnonymousPad128;
typedef struct { char m_pad129; } AnonymousPad129;
typedef struct { char m_pad130; } AnonymousPad130;
typedef struct { char m_pad131; } AnonymousPad131;
typedef struct { char m_pad132; } AnonymousPad132;
typedef struct { char m_pad133; } AnonymousPad133;
typedef struct { char m_pad134; } AnonymousPad134;
typedef struct { char m_pad135; } AnonymousPad135;
typedef struct { char m_pad136; } AnonymousPad136;
typedef struct { char m_pad137; } AnonymousPad137;
typedef struct { char m_pad138; } AnonymousPad138;
typedef struct { char m_pad139; } AnonymousPad139;
typedef struct { char m_pad140; } AnonymousPad140;
typedef struct { char m_pad141; } AnonymousPad141;
typedef struct { char m_pad142; } AnonymousPad142;
typedef struct { char m_pad143; } AnonymousPad143;
typedef struct { char m_pad144; } AnonymousPad144;
typedef struct { char m_pad145; } AnonymousPad145;
typedef struct { char m_pad146; } AnonymousPad146;
typedef struct { char m_pad147; } AnonymousPad147;
typedef struct { char m_pad148; } AnonymousPad148;
typedef struct { char m_pad149; } AnonymousPad149;
typedef struct { char m_pad150; } AnonymousPad150;
typedef struct { char m_pad151; } AnonymousPad151;
typedef struct { char m_pad152; } AnonymousPad152;
typedef struct { char m_pad153; } AnonymousPad153;
typedef struct { char m_pad154; } AnonymousPad154;
typedef struct { char m_pad155; } AnonymousPad155;
typedef struct { char m_pad156; } AnonymousPad156;
typedef struct { char m_pad157; } AnonymousPad157;
typedef struct { char m_pad158; } AnonymousPad158;
typedef struct { char m_pad159; } AnonymousPad159;
typedef struct { char m_pad160; } AnonymousPad160;
typedef struct { char m_pad161; } AnonymousPad161;
typedef struct { char m_pad162; } AnonymousPad162;
typedef struct { char m_pad163; } AnonymousPad163;
typedef struct { char m_pad164; } AnonymousPad164;
typedef struct { char m_pad165; } AnonymousPad165;
typedef struct { char m_pad166; } AnonymousPad166;
typedef struct { char m_pad167; } AnonymousPad167;
typedef struct { char m_pad168; } AnonymousPad168;
typedef struct { char m_pad169; } AnonymousPad169;
typedef struct { char m_pad170; } AnonymousPad170;
typedef struct { char m_pad171; } AnonymousPad171;
typedef struct { char m_pad172; } AnonymousPad172;
typedef struct { char m_pad173; } AnonymousPad173;
typedef struct { char m_pad174; } AnonymousPad174;
typedef struct { char m_pad175; } AnonymousPad175;
typedef struct { char m_pad176; } AnonymousPad176;
typedef struct { char m_pad177; } AnonymousPad177;
typedef struct { char m_pad178; } AnonymousPad178;
typedef struct { char m_pad179; } AnonymousPad179;
typedef struct { char m_pad180; } AnonymousPad180;
typedef struct { char m_pad181; } AnonymousPad181;
typedef struct { char m_pad182; } AnonymousPad182;
typedef struct { char m_pad183; } AnonymousPad183;
typedef struct { char m_pad184; } AnonymousPad184;
typedef struct { char m_pad185; } AnonymousPad185;
typedef struct { char m_pad186; } AnonymousPad186;
typedef struct { char m_pad187; } AnonymousPad187;
typedef struct { char m_pad188; } AnonymousPad188;
typedef struct { char m_pad189; } AnonymousPad189;
typedef struct { char m_pad190; } AnonymousPad190;
typedef struct { char m_pad191; } AnonymousPad191;
typedef struct { char m_pad192; } AnonymousPad192;
typedef struct { char m_pad193; } AnonymousPad193;
typedef struct { char m_pad194; } AnonymousPad194;
typedef struct { char m_pad195; } AnonymousPad195;
typedef struct { char m_pad196; } AnonymousPad196;
typedef struct { char m_pad197; } AnonymousPad197;
typedef struct { char m_pad198; } AnonymousPad198;
typedef struct { char m_pad199; } AnonymousPad199;
typedef struct { char m_pad200; } AnonymousPad200;
typedef struct { char m_pad201; } AnonymousPad201;
typedef struct { char m_pad202; } AnonymousPad202;
typedef struct { char m_pad203; } AnonymousPad203;
typedef struct { char m_pad204; } AnonymousPad204;
typedef struct { char m_pad205; } AnonymousPad205;
typedef struct { char m_pad206; } AnonymousPad206;
typedef struct { char m_pad207; } AnonymousPad207;
typedef struct { char m_pad208; } AnonymousPad208;
typedef struct { char m_pad209; } AnonymousPad209;
typedef struct { char m_pad210; } AnonymousPad210;
typedef struct { char m_pad211; } AnonymousPad211;
typedef struct { char m_pad212; } AnonymousPad212;
typedef struct { char m_pad213; } AnonymousPad213;
typedef struct { char m_pad214; } AnonymousPad214;
typedef struct { char m_pad215; } AnonymousPad215;
typedef struct { char m_pad216; } AnonymousPad216;
typedef struct { char m_pad217; } AnonymousPad217;
typedef struct { char m_pad218; } AnonymousPad218;
typedef struct { char m_pad219; } AnonymousPad219;
typedef struct { char m_pad220; } AnonymousPad220;
typedef struct { char m_pad221; } AnonymousPad221;
typedef struct { char m_pad222; } AnonymousPad222;
typedef struct { char m_pad223; } AnonymousPad223;
typedef struct { char m_pad224; } AnonymousPad224;
typedef struct { char m_pad225; } AnonymousPad225;
typedef struct { char m_pad226; } AnonymousPad226;
typedef struct { char m_pad227; } AnonymousPad227;
typedef struct { char m_pad228; } AnonymousPad228;
typedef struct { char m_pad229; } AnonymousPad229;
typedef struct { char m_pad230; } AnonymousPad230;
typedef struct { char m_pad231; } AnonymousPad231;
typedef struct { char m_pad232; } AnonymousPad232;
typedef struct { char m_pad233; } AnonymousPad233;
typedef struct { char m_pad234; } AnonymousPad234;
typedef struct { char m_pad235; } AnonymousPad235;
typedef struct { char m_pad236; } AnonymousPad236;
typedef struct { char m_pad237; } AnonymousPad237;
typedef struct { char m_pad238; } AnonymousPad238;
typedef struct { char m_pad239; } AnonymousPad239;
typedef struct { char m_pad240; } AnonymousPad240;
typedef struct { char m_pad241; } AnonymousPad241;
typedef struct { char m_pad242; } AnonymousPad242;
typedef struct { char m_pad243; } AnonymousPad243;
typedef struct { char m_pad244; } AnonymousPad244;
typedef struct { char m_pad245; } AnonymousPad245;
typedef struct { char m_pad246; } AnonymousPad246;
typedef struct { char m_pad247; } AnonymousPad247;
typedef struct { char m_pad248; } AnonymousPad248;
typedef struct { char m_pad249; } AnonymousPad249;
typedef struct { char m_pad250; } AnonymousPad250;
typedef struct { char m_pad251; } AnonymousPad251;
typedef struct { char m_pad252; } AnonymousPad252;
typedef struct { char m_pad253; } AnonymousPad253;
typedef struct { char m_pad254; } AnonymousPad254;
typedef struct { char m_pad255; } AnonymousPad255;
typedef struct { char m_pad256; } AnonymousPad256;
typedef struct { char m_pad257; } AnonymousPad257;
typedef struct { char m_pad258; } AnonymousPad258;
typedef struct { char m_pad259; } AnonymousPad259;
typedef struct { char m_pad260; } AnonymousPad260;
typedef struct { char m_pad261; } AnonymousPad261;
typedef struct { char m_pad262; } AnonymousPad262;
typedef struct { char m_pad263; } AnonymousPad263;
typedef struct { char m_pad264; } AnonymousPad264;
typedef struct { char m_pad265; } AnonymousPad265;
typedef struct { char m_pad266; } AnonymousPad266;
typedef struct { char m_pad267; } AnonymousPad267;
typedef struct { char m_pad268; } AnonymousPad268;
typedef struct { char m_pad269; } AnonymousPad269;
typedef struct { char m_pad270; } AnonymousPad270;
typedef struct { char m_pad271; } AnonymousPad271;
typedef struct { char m_pad272; } AnonymousPad272;
typedef struct { char m_pad273; } AnonymousPad273;
typedef struct { char m_pad274; } AnonymousPad274;
typedef struct { char m_pad275; } AnonymousPad275;
typedef struct { char m_pad276; } AnonymousPad276;
typedef struct { char m_pad277; } AnonymousPad277;
typedef struct { char m_pad278; } AnonymousPad278;
typedef struct { char m_pad279; } AnonymousPad279;
typedef struct { char m_pad280; } AnonymousPad280;
typedef struct { char m_pad281; } AnonymousPad281;
typedef struct { char m_pad282; } AnonymousPad282;
typedef struct { char m_pad283; } AnonymousPad283;
typedef struct { char m_pad284; } AnonymousPad284;
typedef struct { char m_pad285; } AnonymousPad285;
typedef struct { char m_pad286; } AnonymousPad286;
typedef struct { char m_pad287; } AnonymousPad287;
typedef struct { char m_pad288; } AnonymousPad288;
typedef struct { char m_pad289; } AnonymousPad289;
typedef struct { char m_pad290; } AnonymousPad290;
typedef struct { char m_pad291; } AnonymousPad291;
typedef struct { char m_pad292; } AnonymousPad292;
typedef struct { char m_pad293; } AnonymousPad293;
typedef struct { char m_pad294; } AnonymousPad294;
typedef struct { char m_pad295; } AnonymousPad295;
typedef struct { char m_pad296; } AnonymousPad296;
typedef struct { char m_pad297; } AnonymousPad297;
typedef struct { char m_pad298; } AnonymousPad298;
typedef struct { char m_pad299; } AnonymousPad299;
typedef struct { char m_pad300; } AnonymousPad300;
typedef struct { char m_pad301; } AnonymousPad301;
typedef struct { char m_pad302; } AnonymousPad302;
typedef struct { char m_pad303; } AnonymousPad303;
typedef struct { char m_pad304; } AnonymousPad304;
typedef struct { char m_pad305; } AnonymousPad305;
typedef struct { char m_pad306; } AnonymousPad306;
typedef struct { char m_pad307; } AnonymousPad307;
typedef struct { char m_pad308; } AnonymousPad308;
typedef struct { char m_pad309; } AnonymousPad309;
typedef struct { char m_pad310; } AnonymousPad310;
typedef struct { char m_pad311; } AnonymousPad311;
typedef struct { char m_pad312; } AnonymousPad312;
typedef struct { char m_pad313; } AnonymousPad313;
typedef struct { char m_pad314; } AnonymousPad314;
typedef struct { char m_pad315; } AnonymousPad315;
typedef struct { char m_pad316; } AnonymousPad316;
typedef struct { char m_pad317; } AnonymousPad317;
typedef struct { char m_pad318; } AnonymousPad318;
typedef struct { char m_pad319; } AnonymousPad319;
typedef struct { char m_pad320; } AnonymousPad320;
typedef struct { char m_pad321; } AnonymousPad321;
typedef struct { char m_pad322; } AnonymousPad322;
typedef struct { char m_pad323; } AnonymousPad323;
typedef struct { char m_pad324; } AnonymousPad324;
typedef struct { char m_pad325; } AnonymousPad325;
typedef struct { char m_pad326; } AnonymousPad326;
typedef struct { char m_pad327; } AnonymousPad327;
typedef struct { char m_pad328; } AnonymousPad328;
typedef struct { char m_pad329; } AnonymousPad329;
typedef struct { char m_pad330; } AnonymousPad330;
typedef struct { char m_pad331; } AnonymousPad331;
typedef struct { char m_pad332; } AnonymousPad332;
// ==================== /ANON-PAD ====================
#include "DisJoint.h"
#include "StrikerScript.h"
#include "NpcScript.h"          // STScriptFileList 完整定义（create 用）
#include "CharacManageScript.h"
#include "CItemUpgrade_Separate.h"
#include "STStackableScript.h"        // ENUM_MODULE_TYPE（DataManagerInitDepends 前置要求）
#include "DataManagerInitDepends.h"   // SDCActiveStatusSyncServer / WongWork::CCompoundAvatar
#include "PvPTypes.h"                 // CMTRand
#include "DNFLexWrapper.h"            // ScanType/ScanInt/ScanFloat/ScanStr/loadRDARScriptFile

#include <cstdio>
#include <ctime>
#include <new>
#include <string>
#include <vector>

// ============================ DisjointScript ============================


#include "CtorProducers_2/LoadDisJoint.inc"
{
    // [SweepCtorProd2] gap −1→+1（78/77 条），jcc 多重集全等（call 6/je 1/jmp 3/
    // jne 2/setg 1/setle 1，逐条核对落点体一致），p167 CLEAR（6=6）。
    // 残余 1 条 = 循环尾基址/索引装载寄存器角色互换（ORIG `mov i,%edx; mov this,%eax;
    // add $0x5c,%edx; shl $4,%edx; add %edx,%eax` vs 4.4 `mov i,%eax; … add this,%eax`）
    // ⇒ N7 族（间接/地址装载寄存器角色，非源形可控），语义等价。
    // 已修：删 `const int count` 缓存（ORIG 内层条件每轮重算 m_additional[i].m_count，
    // 缓存版多发一段循环外计算并多占一槽使全帧深 4）。
    // ORIG 0x0847300c：import 成功则把 [cube index] 第 5 组首值与其
    // [additional result] 各组元素聚合到 +0x628 起的 6 个 vector。
    if (!importDisjointScript(this, "Etc/Disjoint.etc"))
    {
        std::puts("DisJoint Script Error: Etc/Disjoint.etc");
        return false;
    }

    for (int i = 0; i < 6; ++i)
    {
        int value = m_cubeIndexVecs[4].at(0);        // at(this+0x30, 0)；ORIG 显式 _ZNSt6vectorIiSaIiEE2atEj
        m_resultVecs[i].push_back(value);
        // [SweepCtorProd2] ORIG 0x084730xx 内层条件每轮重算（`mov i,%edx; add $0x5c;
        // shl $4; add this; add $8; mov (%eax),%eax; cmp j,%eax; setg`），无 count
        // 缓存局部；原 `const int count` 缓存多发一段循环外计算并多占一个槽
        // （全帧槽深 +4）。表达式内联进 for 条件即复现。
        // [M5F_CtorDB 2026-09-11] 唯一残差（ORIG 78 / 我方 77）= 循环尾基址装载：
        // ORIG `mov -0x10,%edx; mov 0x8(%ebp),%eax; add $0x5c,%edx; shl $4,%edx;
        // add %edx,%eax`（**索引先落 %edx、this 后落 %eax 再 add**），我方 4.4 折成
        // `mov i,%eax; …; add 0x8(%ebp),%eax`（内存操作数直加，少 1 条 mov）。
        // 本轮换「成员访问形」试造 ORIG 的装载序：m_additional[i].m_count 走
        // 索引强度削减 `(i+0x5c)<<4 + 8`，与 ORIG 的 add $0x5c/shl $4 逐条同源；
        // 语义等价、布局/mangled 零变化。若读数仍 77 ⇒ 归 N7（寄存器角色，4.1 独有）。
        for (int j = 0; j < m_additional[i].m_count; ++j)
        {
            m_resultVecs[i].push_back(m_additional[i].m_items[j]);
        }
    }
    return true;
}

// ---- DisjointSkillScript（ORIG W 0x084735fe/0x084735d2；布局 4×int）----
#include "CtorProducers_2/DisjointSkillScript.inc"
{
    // ORIG 0x084735fe：仅调用 clear()。
    clear();
}

#include "CtorProducers_2/clear.inc"
{
    // ORIG 0x084735d2：{0xb, 0, 0, 4} 四连写。
    // 定义置于 ctor 之后：4.4 对"被调函数本 TU 未定义"的 ctor 帧发 sub $0x18，
    // 与 ORIG 0x084735fe 帧形一致（探针 /tmp/near7/p7-p8）。
    m_field00 = 0xb;
    m_field04 = 0;
    m_field08 = 0;
    m_field0c = 4;
}

// ---- DisjointScript::._381（真匿名 struct，DisJoint.h 内 m_additional[6]）----
// ORIG ctor _ZN14DisjointScript5._381C1Ev t 0x08348088 (0x16)：仅构造 +4 vector<int>；
// dtor _ZN14DisjointScript5._381D1Ev t 0x0834809e：逆序析构 vector。
// [P203 2026-09-11 GiantB] 原「命名类 t_381 + asm(".set")」改为编译器自然发射
// （`.set` 改不了重定位名，AnonLane 实证）⇒ 本文件不再显式定义 ctor/dtor，
// 由 DisJoint.h 的匿名 struct 自动合成；编号由顶部 ANON-PAD 垫到 381。

#include "CtorProducers_2/DisjointScript.inc"
{
    // ORIG 0x08375d4c：成员容器自动构造（含 6×匿名 _381@+0x5c8 循环调其 C1），
    // 函数体仅调 clear()；m_skillSelect/m_fieldCC/m_creationConst 的初值由
    // clear()（0x08375bca）写入，ctor 不再重复赋值（ORIG 实证无这些 store）。
    clear();
}

#include "CtorProducers_2/clear_1.inc"
{
    // [SweepCtorProd2] 88→119 条 vs ORIG 120：唯一残余 = ORIG 尾部对齐 nop（§6.1
    // 豁免）⇒ AE。p167 CLEAR（3=3）。
    // 已补：① ORIG 0x08375cf3..0x8375d42 parts 段（grade 0..3 × parts 0..9，
    // `movl $0,0x4(%ecx,%eax,4)`、eax=0x110+g*10+p ⇒ 基准 +0x444）；
    // ② 0xcc/0xd0 的 2 元素循环（ORIG 0x8375c5d..0x8375c7c `lea 0x30(%edx),%ecx;
    // mov %edx,0xc(%eax,%ecx,4)`，经 DisJoint.h 匿名 union 数组视角 m_skillProb[2] 复现）。
    // ORIG 0x08375bca：5 容器 clear → 4 LotteryInfo::clear → +0xbc=-1 →
    // skills clear → (+0xcc,+0xd0) 循环清零 → 三重循环 m_avatarDisjoint →
    // 二重循环 m_avatarPartsDisjoint。
    // [b28d-48] P191 配套：ORIG 0x08375bca 双 `movl $0,-0x20`（`int i=0;`
    // 与 `for(i=0;…)` 两条初始化都发射），for-init 声明形只发一条。
    int i = 0;
    for (i = 0; i < 5; ++i)
    {
        m_cubeIndexVecs[i].clear();
    }
    for (i = 0; i < 4; ++i)
    {
        m_legacyInfo[i].clear();
    }
    m_skillSelect = -1;
    m_skills.clear();
    // ORIG 0x08375bca 续：i=0..1 清零 (+0xcc,+0xd0) 两个 float——ORIG 是 2 元素
    // 循环 `mov i,%edx; mov this,%eax; lea 0x30(%edx),%ecx; mov $0,%edx;
    // mov %edx,0xc(%eax,%ecx,4)`（this+0xcc+4i，成员数组寻址）⇒ 用头内匿名 union
    // 的数组视角 m_skillProb[2] 循环访问（原两条独立赋值少 6 条且无槽复用）。
    for (i = 0; i <= 1; ++i)
    {
        m_skillProb[i] = 0;
    }
    // 随后三重循环 a(job)=0..10、b(grade)=0..3、c(slot)=0..4 清零
    // m_avatarDisjoint[a][b][c]（索引式 (c+b*5+a*20+0x34)，字节区 +0xd4..+0x443）。
    for (int a = 0; a < 11; ++a)
    {
        for (int b = 0; b < 4; ++b)
        {
            for (int c = 0; c < 5; ++c)
            {
                m_avatarDisjoint[a][b][c] = 0;
            }
        }
    }
    // ORIG 0x08375cf3..0x08375d42（SweepScriptDepsA 证据）：grade 0..3 × parts 0..9
    // 清零 m_avatarPartsDisjoint。寻址 `movl $0x0,0x4(%ecx,%eax,4)`、eax=0x110+g*10+p
    // ⇒ 基准 4+0x110*4 = 0x444，与头 [4][10]@+0x444 一致。
    for (int grade = 0; grade < 4; ++grade)
    {
        for (int parts = 0; parts < 10; ++parts)
        {
            m_avatarPartsDisjoint[grade][parts] = 0;
        }
    }
}

#include "CtorProducers_2/dtor_DisjointScript.inc"
{
    // ORIG 0x0837dff8：逆序析构各容器成员。
}

// ============================ DisJoint ============================

#include "CtorProducers_2/DisJoint.inc"
{
    // ORIG 0x0837e532：基类构造 + 6×vector<int> @+0x628 默认构造。
}

#include "CtorProducers_2/dtor_DisJoint.inc"
{
    // ORIG 0x0837dff8 链：基类析构 + 6 个结果 vector 析构。
}

// ============================ StrikerScript ============================


// ORIG 静态成员单例指针（nm B 数据；getInsance 懒加载落点）。
StrikerScript* StrikerScript::instance_ = 0;              // _ZN13StrikerScript9instance_E B 0x09500978
CharacManageScript* CharacManageScript::instance_ = 0;    // _ZN18CharacManageScript9instance_E B 0x094f6b98


#include "CtorProducers_2/getInsance.inc"
{
    // ORIG 0x08a9d836：懒加载单例（new 0xb4 → ctor → 存 instance_）。
    if (instance_ == 0)
    {
        instance_ = new StrikerScript;
    }
    return instance_;
}

#include "CtorProducers_2/clear_2.inc"
{
    // ORIG 0x08a9dbb4：标量复位 + 容器 clear。
    m_tagCostTime = 0;
    m_tagCharacterSetting = 0;
    m_strikerCombo = 0;
    m_strikerStartTime = 0;
    m_strikerEndTime = 0x1f4;                    // 500
    m_moduleImpossible.clear();
    m_dungeonImpossible.clear();
    m_field38 = 0;
    m_field3c = 0;
    m_fileList = 0;                              // +0x90
    m_loaded = 0;                                // +0xa0 byte
    m_field64 = 0;                               // +0x64
    m_field68 = 0;                               // +0x68
    m_commonCoolTime = 0;                        // +0xa4
    m_positionType = 0;                          // +0xac
    m_upgradeRevision = 0x63;                    // +0xb0 = 99
}

#include "CtorProducers_2/create.inc"
{
    // ORIG 0x08a9dc70：new STScriptFileList(0x18) → +0x90。
    m_fileList = new STScriptFileList();
}

#include "CtorProducers_2/destory.inc"
{
    // [b28d-48] N82 封顶（NDIFF 65，69/72 条）：ORIG 先 `mov 0x90(%eax),%ebx`
    // 再判空（直接读成员）vs 4.4 先 mov 缓存 eax 再判空；帧/槽平移。
    // ORIG 0x08a9dcbe：delete 文件列表（STScriptFileList::D1 + operator delete）；
    // 遍历技能 map delete 各 vector 并置空；尾部 m_skillMap.clear()。
    // [SweepCtorProd2] 原判「二次装载不可造」作废：ORIG 的两次 `mov 0x90(%eax)`
    // 是「源码显式 if 判空」+「delete 自带空指针守卫」两段（探针实测 4.4 同形）⇒
    // 补显式 if 即逐条复现（`mov 0x90(%eax),%eax; test; je` + `mov 0x90(%eax),%ebx;
    // test %ebx,%ebx; je`）。
    if (m_fileList != 0)
    {
        delete m_fileList;                       // P301：具型指针单段直落
    }
    m_fileList = 0;
    // [b28c-lane17] ORIG 0x08a9dce8：循环谓词每次调用都**重新装载** m_fileList
    // 的迭代器对（`mov this→%eax; mov 0x90(%eax),%eax; test; ...; mov 0x90(%eax),%ebx`
    // 两条独立装载），且 delete 前无二次解引用。原写 `x = m_fileList; if (x)`
    // 被 4.4 折成单段直落（少 1 条）。
    for (std::map<int, std::vector<StrikerSkill>*>::iterator it = m_skillMap.begin();
         it != m_skillMap.end(); ++it)
    {
        if (it->second != 0)
        {
            delete it->second;
        }
        // [SweepCtorProd2·jmp 目标下标判据] ORIG 0x08a9dd9c 的 `movl $0,0x4(%eax)`
        // 在 if **外**（无条件重写 0；Ghidra C `if(...){delete} *(iVar2+4)=0;` 实证）：
        // ORIG `je +0x91`（置 0 块）vs 原写 if 内 `je +0xa3`（跳过置 0）——目标不同，
        // 旧 AE 归一化（TGT）掩盖了该差异。移出后 je 目标与 ORIG 逐位一致。
        it->second = 0;
    }
    m_skillMap.clear();   // ORIG 尾部 map clear
}



#include "CtorProducers_2/StrikerScript.inc"
{
    // ORIG 0x08a9d88e：各容器默认构造（成员自动）→ clear() → create()。
    clear();
    create();
}

#include "CtorProducers_2/dtor_StrikerScript.inc"
{
    // ORIG 0x08a9da28：destory() 后逆序析构容器成员。
    destory();
}

// ============================ CharacManageScript ============================

#include "CtorProducers_2/getInsance_1.inc"
{
    // ORIG 0x088e0674：懒加载单例（new 0x88 → ctor → 存 instance_）。
    if (instance_ == 0)
    {
        instance_ = new CharacManageScript;
    }
    return instance_;
}

#include "CtorProducers_2/CharacManageScript.inc"
{
    // ORIG 0x088e06cc：vector<ManagePointBonus>@+0、vector<int>@+0xc、
    // map<int,int>@+0x18/+0x30、map<int,string>@+0x4c、map<int,int>@+0x64、
    // vector<ContentsOptionRevision>@+0x7c 默认构造（成员自动）后调 clear()。
    clear();
}

#include "CtorProducers_2/clear_3.inc"
{
    // ORIG 0x088e0904：两 vector clear → +0x18/+0x30 map clear →
    // +0x48=0 → map<int,string> clear → +0x64 map clear（依 asm 顺序）。
    m_pointBonus.clear();
    m_manageLevelPoint.clear();
    m_expBonus.clear();
    m_goldBonus.clear();
    m_manageLevelMax = 0;
    m_basicExplain.clear();
    m_manageOption.clear();
}

#include "CtorProducers_2/dtor_CharacManageScript.inc"
{
    // ORIG 0x088e07f6：逆序析构 +0x7c vector → 4×map → +0xc vector → +0x00
    // vector<ManagePointBonus>（成员自动，顺序一致）。
}

#include "CtorProducers_2/importScript.inc"
{
    // [SweepCtorProd2] 已修 3 个真 bug（p167 47=47 CLEAR，残余 4 项 C1/C2·D1/D2
    // 同址别名 = P93 豁免，nm 实证 ORIG 0x087065d0/0x08707be0/0x088e12f6/0x080c6d20）：
    // ① 8 处 `token == "…"` 实参序反写 → `"…" == token`（ORIG 每点
    //    `movl $"[…]",(%esp); mov %eax,0x4(%esp)` 选 `operator==(const char*,
    //    const string&)`；原写法选到另一重载 `…RKSb…EPKS3_`）；
    // ② `make_pair(key,val)` 的 key/val 去 const（ORIG 实例 `IRiS0_` 非 const 左值，
    //    const 版会实例化 `IRKiS1_` 另一符号）；
    // ③ `[exp bonus]/[gold bonus]` 分支由三目改为循环内 if/else-if + `else return false`
    //    （ORIG 0x88e0b8e..0x88e0be3：选表指针每轮重置、缺第 9 次 operator== 与失败返回）。
    // 帧 0x60→0x70（P232 撑帧，char framePad[0x10]）与 ORIG 对齐。
    // 残余 DIFF = while 布局（4.1 条件在前 fall-through vs 4.4 `jmp cond`）+
    // bool 判断 xor 物化（P168a④/N122：4.4 无 `movzbl;xor$1;test;je` 形）+
    // 解析段槽分配（N82/N93 族）⇒ 语义等价、结构 N 封顶。
    // ORIG 0x088e0968：RDAR 装载失败 ret0；分节扫描 [point bonus]/
    // [manage level point]/[manage level max]/[exp bonus]/[gold bonus]/
    // [manage basic explain]/[manage option]/默认 ContentsOptionRevision 行。
    // [M5F_CtorDB 2026-09-11] our<ORIG（342/329）⇒ 只找「ORIG 有我方无」段；
    // seq-align 差集**完全解析**为 13 条（无一条语义指令）：
    //   ① 4 处 `movzbl ok; xor $0x1,%eax; test %al,%al; je`（4.1 对 `if(!ok)` 先
    //      取反再物化；4.4 直产 `test; jne`）⇒ 每处差 2 条 = 8；
    //   ② 4 条 `nop`（4.1 循环对齐伪指令，落点见 0x088e0a9f/0x088e0c8a/0x088e0d1e/
    //      0x088e0d6f，全部不可达）⇒ 4；
    //   ③ 1 条 while 布局差（4.1 条件在头 + 回边 jmp +0x46，4.4 条件在尾 + `jmp +0x414`）。
    // 新钥匙逐条试过：P333（局部 static 双检锁表）不适用——本件无 .bss 取址/guard；
    // P337 死块——上述 nop 岛**不可达但无指令**（纯对齐），补 nop 无意义；
    // P283/P164 循环落点——`while (ScanType(token,true))` 4.4 恒尾测，改 for 同码。
    // 调用点差集 CLEAR 47=47（父泳道复核）；串/变参序（ERROR [%s][%s], path, token）
    // 与 ORIG 一致（§6.5 已核）。⇒ N 封顶（4.1 xor 物化 + 对齐 nop），语义等价。
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;
    std::string value;
    bool ok = false;
    char framePad[0x10];                         // [SweepCtorProd2] ORIG 帧 0x70 vs 我方 0x60
    (void)framePad;                              // ⇒ 全槽平移 0x10（P232 撑帧实验）
    while (ScanType(token, true))
    {
        // [SweepCtorProd2] 实参序判据（P18/P23 族）：ORIG 每个比较点都是
        // `movl $"[...]",(%esp); mov %eax,0x4(%esp)` ⇒ 字面量在 arg0、token 在 arg1，
        // 选 `operator==(const char*, const string&)`（_ZSteq…bPKT_RKSbIS3_T0_T1_E，
        // ORIG 9 次）；写反成 `token == "…"` 选另一重载（我方原 8 次）⇒ 全部翻正。
        if ("[point bonus]" == token)
        {
            while (true)
            {
                ManagePointBonus bonus;              // ORIG 栈 ctor 0x088e12f6
                bonus.m_field00 = ScanInt(&ok);
                if (!ok)
                {
                    break;                           // 回 ScanType 主循环（ORIG jmp +0x46）
                }
                bonus.m_field04 = ScanInt(&ok);
                if (!ok)
                {
                    return false;
                }
                bonus.m_field08 = ScanInt(&ok);
                if (!ok)
                {
                    return false;
                }
                m_pointBonus.push_back(bonus);
            }
        }
        else if ("[manage level point]" == token)
        {
            int level = 0;
            while ((level = ScanInt(&ok)), ok)       // do-while 形态（asm +0x11a）
            {
                m_manageLevelPoint.push_back(level);
            }
        }
        else if ("[manage level max]" == token)
        {
            m_manageLevelMax = ScanInt(&ok);         // +0x48
            if (!ok)
            {
                return false;
            }
        }
        else if ("[exp bonus]" == token || "[gold bonus]" == token)
        {
            // [SweepCtorProd2] ORIG 0x88e0b42/0x88e0b8e..0x88e0be3：
            // ① val(-0x50) 在循环**外**预清 `movl $0,-0x50`，循环内 `val = ScanInt`；
            // ② key(-0x4c) 每轮新声明；③ 选表指针(-0x10) 在**循环体内**每轮重置：
            // `== "[exp bonus]"` → +0x18，否则 `== "[gold bonus]"` → +0x30，
            // 否则 `mov $0,%ebx; jmp` = return false（原三目写法把这 2 次比较移到
            // 循环外 ⇒ p167 少 1 次 operator==、少失败返回路径）。
            int val = 0;
            while (true)
            {
                // ORIG make_pair 实例 = `IRiS0_`（int&，非 const 左值）⇒ key/val 必须
                // 是非 const 局部（P61/P315：const 左值会实例化 IRKiS1_ 另一符号）。
                int key = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                val = ScanInt(&ok);
                if (!ok)
                {
                    return false;
                }
                std::map<int, int>* table = 0;
                if ("[exp bonus]" == token)
                {
                    table = &m_expBonus;             // +0x18
                }
                else if ("[gold bonus]" == token)
                {
                    table = &m_goldBonus;            // +0x30
                }
                else
                {
                    return false;
                }
                const std::pair<std::map<int, int>::iterator, bool> ins =
                    table->insert(std::make_pair(key, val));
                if (!ins.second)                     // insert.second==0 → 查重告警
                {
                    std::printf("ERROR [%s][%s] key duplicated\n",
                                path, token.c_str());
                }
            }
        }
        else if ("[manage basic explain]" == token)
        {
            m_basicExplain.clear();                  // 先 clear 再按行号写入
            int index = 0;
            while (ScanStr(&value))
            {
                m_basicExplain[index] = value;
                ++index;
            }
        }
        else if ("[manage option]" == token)
        {
            m_manageOption.clear();
            while (true)
            {
                const int key = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                m_manageOption[key] = ScanInt((bool*)0);   // ORIG ScanIntPb(NULL)
            }
        }
        else
        {
            m_optionRevision.clear();
            while (ScanStr(&value))
            {
                ContentsOptionRevision rev;          // POD {int type; int rev;}
                rev.m_type = ContentsType::getType(value.c_str());
                rev.m_field04 = ScanInt((bool*)0);
                m_optionRevision.push_back(rev);
            }
        }
    }
    return true;                                     // ScanType 耗尽 ret1
}

// ============================ SDCActiveStatusSync Common/Server ============================

#include "CtorProducers_2/SDCActiveStatusSyncCommon.inc"
{
    // ORIG W 0x08234870：空体 ret。
}

#include "CtorProducers_2/dtor_SDCActiveStatusSyncCommon.inc"
{
    // ORIG W 0x08234876：空体 ret。
}

#include "CtorProducers_2/loadActiveStatusScript.inc"
{
    // ORIG T 0x0808e13c：尾透传 return importActiveStatusTable("Etc/ActiveStatusTable_ex.etc")。
    return importActiveStatusTable("Etc/ActiveStatusTable_ex.etc");
}

// [P203 2026-09-11 GiantB] 原「命名类 t_381 + `.globl`+`.set` 别名」块已**删除**：
// `.set` 只改 nm 轴/反汇编标注名，改不了**重定位目标名**（我方 call 写 `5t_381C1Ev`，
// ORIG 是 `5._381C1Ev` ⇒ p167 恒 MISSING/EXTRA，AnonLane 实证）。现由 DisJoint.h 的
// 真匿名 struct 自然发射 `_ZN14DisjointScript5._381{C1,C2,D1,D2}Ev`（编号由顶部
// ANON-PAD 垫到 381：声明点自然编号 48 + 333）。orphan：CtorProducers_2/t_381.inc、
// dtor_t_381.inc 已不再被任何 TU include（可删）。
