#include "emulator.hpp"
#include "gtest/gtest.h"

#if 0 
int main() {
    Memory mem;
    CPU cpu;
    cpu.reset(mem);

//prog start
    mem[0xFFFC] = CPU::INS_JSR;
    mem[0xFFFD] = 0x42;
    mem[0xFFFE] = 0x42;
    mem[0x4242] = CPU::INS_LDA_IM;
    mem[0x4243] = 0x84;
//prog end

    cpu.execute(9, mem);
    return 0;
}
#endif

#if defined(GTEST_OS_ESP8266) || defined(GTEST_OS_ESP32) || \
    (defined(GTEST_OS_NRF52) && defined(ARDUINO))
// Arduino-like platforms: program entry points are setup/loop instead of main.

#ifdef GTEST_OS_ESP8266
extern "C" {
#endif

void setup() { testing::InitGoogleTest(); }

void loop() { RUN_ALL_TESTS(); }

#ifdef GTEST_OS_ESP8266
}
#endif

#elif defined(GTEST_OS_QURT)
// QuRT: program entry point is main, but argc/argv are unusable.

GTEST_API_ int main() {
  printf("Running main() from %s\n", __FILE__);
  testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}
#else
// Normal platforms: program entry point is main, argc/argv are initialized.

GTEST_API_ int main(int argc, char **argv) {
  printf("Running main() from %s\n", __FILE__);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
#endif
