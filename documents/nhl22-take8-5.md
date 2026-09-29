# NHL 22 Take 8.5 experimental matrix

Build on Take 7's visible Press X prompt and Take 8's input, fence, flip, and HDR tracing. These switches are diagnostic experiments, not compatibility fixes. Each run should use the same game files, driver, and emulator settings. Press X once, wait two minutes, and retain the full log.

| Run | Environment set before launching shadPS4 | Question |
| --- | --- | --- |
| Baseline | None | Does the prompt disappear, and where do flips/fences stop? |
| SDR | `SHADPS4_NHL22_FORCE_SDR=1` | Does an SDR swapchain change brightness or menu progress? |
| Occlusion | `SHADPS4_NHL22_OCCLUSION=zero` | Does the game's menu branch depend on the synthetic visible-sample counter? |
| Combined | Both switches | Does the combination differ from either individual run? |

On Windows, open Command Prompt in the extracted build folder, run `set SHADPS4_NHL22_FORCE_SDR=1` and/or `set SHADPS4_NHL22_OCCLUSION=zero`, then launch `shadPS4.exe` from that same prompt. Closing the prompt resets the switches. The SDR switch keeps the guest's reported output mode intact and changes only the host swapchain selection. The occlusion switch returns a valid zero-sample result instead of shadPS4's invented increasing counter; it may affect rendering and is meant to test a hypothesis, not improve visuals.

The Battlefield 4 fork's real Vulkan occlusion queries require coordinated changes in the rasterizer and scheduler. Its readback code is based on a different buffer-cache architecture from this branch. Both need review and adaptation before inclusion; copying either wholesale would risk corrupting the Take 7 startup path.
