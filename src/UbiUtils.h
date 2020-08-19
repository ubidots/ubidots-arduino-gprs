
#ifndef __UBI_UTILS__
#define __UBI_UTILS__

#include <avr/boot.h>

#ifndef SIGRD
  #define SIGRD 5
#endif

#if defined(ARDUINO_ARCH_AVR)
  #if defined(__AVR_ATmega328PB__)
    #define UniqueIDsize 10
  #else
    #define UniqueIDsize 9
  #endif
#endif

#define UniqueIDbuffer UniqueIDsize

class UbiUtils {
 public:
  static void getUniqueID(char* str) {
    uint8_t id[UniqueIDbuffer];
    for (size_t i = 0; i < UniqueIDsize; i++) {
      id[i] = boot_signature_byte_get(0x0E + i + (UniqueIDsize == 9 && i > 5 ? 1 : 0));
    }
    sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X", id[3], id[4], id[5], id[6], id[7], id[8]);
  }
};  // namespace UbiUtils

#endif
