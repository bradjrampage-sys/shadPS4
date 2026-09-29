# NHL 22 Take 9: post-X front-end probe

Test 8.5 established that the Cross edge reaches `scePadRead`, flips continue beyond 1,980, and EventWriteEop signals continue beyond 178,000. The menu transition is therefore not blocked by the instrumented GPU fence path. The `GfxEopQueue` wait is a long-lived event loop, not evidence of a stalled fence. After Cross, the title registers `sceNpRegisterStateCallbackA` and probes bootflow UI assets. Missing loose `/temp0/*.lreq` files alone do not prove an asset failure because packed CAS files continue to load.

The log also shows `__cxa_guard_acquire` resolving to a stub that always returns zero. This prevents initialization of function-local C++ statics at those call sites. Take 9 implements acquire, release, and abort with atomic state and waiter notification. This is the main behavioral correction, independent of the NHL-specific switches.

Take 9 chooses SDR presentation by default for CUSA26280, which requested PQ in the log and displayed incorrect brightness. This changes the host swapchain choice; the title's guest output mode is unchanged. The previous `SHADPS4_NHL22_FORCE_SDR` option remains available for other titles.

The optional `SHADPS4_NHL22_INITIAL_NP_STATE=1` probe queues a signed-out state when NHL 22 registers a late NP state callback. The regular callback check dispatches it on the game's thread. This tests whether the bootflow is waiting for an initial profile state that was never delivered. It is deliberately opt-in because the correct PS4 callback semantics are not established by this log.

Run baseline once, then close the emulator. To run the NP probe, open Command Prompt in the extracted folder, type `set SHADPS4_NHL22_INITIAL_NP_STATE=1`, launch `shadPS4.exe` from that prompt, press X once, and wait two minutes. Keep each full log. The previous occlusion switch remains available, but the baseline log rules out a GPU fence stall and provides no reason to prioritize it.
