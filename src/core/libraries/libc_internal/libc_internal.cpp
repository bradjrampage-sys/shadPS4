// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <atomic>

#include <common/va_ctx.h>
#include "common/assert.h"
#include "common/logging/log.h"
#include "core/libraries/error_codes.h"
#include "core/libraries/libs.h"
#include "libc_internal.h"
#include "libc_internal_io.h"
#include "libc_internal_math.h"
#include "libc_internal_memory.h"
#include "libc_internal_str.h"
#include "libc_internal_threads.h"
#include "printf.h"

namespace Libraries::LibcInternal {

// The first byte of an Itanium C++ ABI guard is set only after initialization.
// Use a private busy bit in the second byte to serialize contending guest threads.
static constexpr u64 GuardInitialized = 1;
static constexpr u64 GuardBusy = 0x100;

int PS4_SYSV_ABI CxaGuardAcquire(u64* guard) {
    std::atomic_ref<u64> state(*guard);
    for (;;) {
        u64 observed = state.load(std::memory_order_acquire);
        if (observed & GuardInitialized) {
            return 0;
        }
        if (observed & GuardBusy) {
            state.wait(observed, std::memory_order_relaxed);
            continue;
        }
        if (state.compare_exchange_weak(observed, observed | GuardBusy,
                                        std::memory_order_acquire)) {
            return 1;
        }
    }
}

void PS4_SYSV_ABI CxaGuardRelease(u64* guard) {
    std::atomic_ref<u64> state(*guard);
    state.store(GuardInitialized, std::memory_order_release);
    state.notify_all();
}

void PS4_SYSV_ABI CxaGuardAbort(u64* guard) {
    std::atomic_ref<u64> state(*guard);
    state.store(0, std::memory_order_release);
    state.notify_all();
}

void RegisterLib(Core::Loader::SymbolsResolver* sym) {
    LIB_FUNCTION("3GPpjQdAMTw", "libSceLibcInternal", 1, "libSceLibcInternal", CxaGuardAcquire);
    LIB_FUNCTION("9rAeANT2tyE", "libSceLibcInternal", 1, "libSceLibcInternal", CxaGuardRelease);
    LIB_FUNCTION("2emaaluWzUw", "libSceLibcInternal", 1, "libSceLibcInternal", CxaGuardAbort);
    RegisterlibSceLibcInternalMath(sym);
    RegisterlibSceLibcInternalStr(sym);
    RegisterlibSceLibcInternalMemory(sym);
    RegisterlibSceLibcInternalIo(sym);
    RegisterlibSceLibcInternalThreads(sym);
}

} // namespace Libraries::LibcInternal
