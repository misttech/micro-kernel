#include <dev/power/psci.h>
#if WITH_LIB_CONSOLE
#include <lk/console_cmd.h>
#include <stdio.h>
#endif

#define PSCI_VERSION  0x84000000
#define SYSTEM_OFF    0x84000008
#define SYSTEM_RESET  0x84000009

#if ARCH_ARM
#define CPU_ON        0x84000003
#endif

#if ARCH_ARM64
#define CPU_ON        0xC4000003
#endif

uint32_t psci_version() {
  return psci_call(PSCI_VERSION, 0, 0, 0);
}

int psci_cpu_on(int corenr, ulong entrypoint) {
  return psci_call(CPU_ON, corenr, entrypoint, corenr);
}

void psci_system_off() {
  psci_call(SYSTEM_OFF, 0, 0, 0);
}

void psci_system_reset() {
  psci_call(SYSTEM_RESET, 0, 0, 0);
}

#if WITH_LIB_CONSOLE

static int cmd_psci_version(int argc, const console_cmd_args *argv) {
  int ret = psci_version();
  printf("PSCI VERSION: 0x%x\n", ret);
  return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("psci_version", "show psci version", &cmd_psci_version)
STATIC_COMMAND_END(psci);
#endif
