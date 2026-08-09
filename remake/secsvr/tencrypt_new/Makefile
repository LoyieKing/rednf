CC      = g++
# -fabi-version=2：GCC 4.1 对类外定义的构造/析构生成 C1/C2 双符号，
# GCC 13 默认只生成 C1，故显式指定以对齐二进制的 mangled 符号表。
CFLAGS  = -m32 -O2 -DNDEBUG -pthread -I. -Wall -fabi-version=2
OBJS    = src/md5.o src/sha1.o src/md2.o src/md4.o src/crc32.o src/adler32.o src/ripemd160.o src/ripemd128.o src/haval160.o src/haval128.o src/gosthash.o src/TenHash.o src/rc4.o src/tean.o src/idea.o src/gost.o src/des.o src/3way.o src/rc5.o src/rc6.o src/rc2.o src/safer.o src/mmb.o src/mbc.o src/cast.o src/loki91.o src/feal8.o src/nsea.o src/loki89.o src/mars.o src/diamond.o src/frog.o src/hpc.o src/gene.o src/seattos.o src/adder.o src/subtracter.o src/shift.o src/ring.o src/gramary.o src/desnew.o src/blowfish.o src/ts_local.o src/d3des.o src/des2p.o src/despc.o src/destoo.o
LIB     = libtencrypt_new.a

all: $(LIB)

$(LIB): $(OBJS)
	ar rcs $@ $^

src/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

test: test_hash.cpp $(LIB)
	$(CC) $(CFLAGS) test_hash.cpp $(LIB) -o test_hash

clean:
	rm -f src/*.o $(LIB) test_hash
