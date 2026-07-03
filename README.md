# ReverseFlow

A professional real-time reverse reverb plugin built with JUCE.

## Features

- Real-time reverse reverb without track duplication or manual reversing
- 5 processing modes: Tight, Vocal, Dream, Cinema, Infinite
- Smart detection analyzes incoming audio and recommends settings
- Tempo sync for reverse time
- Granular freeze for experimental textures
- Full parameter automation and preset management
- 64-bit processing with zero allocations on the audio thread

## Building

### Prerequisites

- CMake 3.22+
- C++20 compiler
- JUCE cloned into `ReverseFlow/JUCE/`

### macOS / Linux

```bash
git clone https://github.com/juce-framework/JUCE.git
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
```

### Windows

```powershell
git clone https://github.com/juce-framework/JUCE.git
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

## Installation

Build artifacts are copied to standard plugin directories automatically (`COPY_PLUGIN_AFTER_BUILD TRUE`).

- VST3: `build/ReverseFlow_artefacts/Release/VST3/`
- AU: `build/ReverseFlow_artefacts/Release/AU/`
- Standalone: `build/ReverseFlow_artefacts/Release/Standalone/`

## DSP Architecture

Input -> Circular Buffer -> Reverse Reader -> Reverb -> Envelope -> EQ -> Saturation -> Mix -> Limiter -> Output

Latency is reported to the host for automatic compensation.

## Controls

| Control      | Range       | Description                          |
| ------------ | ----------- | ------------------------------------ |
| Mode         | 5 modes     | Macro for reverb character           |
| Mix          | 0-100%      | Wet/dry balance                      |
| Reverse Time | 0.1-5.0s    | Look-ahead buffer / reverse duration |
| Decay        | 0.1-10.0s   | Reverb tail length                   |
| Size         | Small-Huge  | Room size                            |
| Pre Delay    | 0-500ms     | Delay before reverse swell           |
| Width        | 0-200%      | Stereo width                         |
| Tone         | Dark-Bright | High-shelf EQ                        |
| Duck         | 0-100%      | Amount of transient ducking          |
| Freeze       | On/Off      | Capture and granulate buffer         |

## License

MIT
