/* Per-TU __FILE__ override for the point reconstruction build.
 *
 * ORIG asserts embed short file names / relative paths (e.g. "Token.cpp",
 * "../../Include/Core/SecureStdio.cpp") because the original build invoked
 * the compiler with those paths.  The CMake reconstruction compiles with
 * absolute paths, so __FILE__ expands to /home/<user>/... and every assert
 * site shows up as a spurious diff.  Each affected TU is compiled with
 * -include <this header> plus -DPOINT_FILEPATH_<TU> so that __FILE__
 * reproduces the exact ORIG string for that translation unit.
 *
 * point 与 auction 同源；ORIG 中这些 TU 的路径字符串与 auction 完全一致
 * （二进制实测），因此本表与 auction_filepath_override.h 相同。
 */
#if defined(POINT_FILEPATH_SECURESTDIO)
#undef __FILE__
#define __FILE__ "../../Include/Core/SecureStdio.cpp"
#elif defined(POINT_FILEPATH_RECVBUFFER)
#undef __FILE__
#define __FILE__ "RecvBuffer.cpp"
#elif defined(POINT_FILEPATH_COMMONDATAPOOL)
#undef __FILE__
#define __FILE__ "CommonDataPool.cpp"
#elif defined(POINT_FILEPATH_TOKEN)
#undef __FILE__
#define __FILE__ "Token.cpp"
#elif defined(POINT_FILEPATH_LOGSENDTHREAD)
#undef __FILE__
#define __FILE__ "LogSendThread.cpp"
#endif
