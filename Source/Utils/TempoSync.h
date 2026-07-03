#pragma once

#include <juce_core/juce_core.h>

namespace TempoSync
{
    enum Note
    {
        SixtyFourth = 0, ThirtySecond, Sixteenth, Eighth, Quarter, Half, Bar, TwoBars,
        DottedSixteenth, DottedEighth, DottedQuarter, TripletEighth, TripletQuarter, NumNotes
    };

    inline float noteToSeconds(Note note, double bpm)
    {
        if (bpm <= 0.0)
            return 0.0f;

        const float quarterSec = 60.0f / static_cast<float>(bpm);
        float factor = 1.0f;

        switch (note)
        {
            case SixtyFourth: factor = 1.0f / 16.0f; break;
            case ThirtySecond: factor = 1.0f / 8.0f; break;
            case Sixteenth: factor = 1.0f / 4.0f; break;
            case Eighth: factor = 1.0f / 2.0f; break;
            case Quarter: factor = 1.0f; break;
            case Half: factor = 2.0f; break;
            case Bar: factor = 4.0f; break;
            case TwoBars: factor = 8.0f; break;
            case DottedSixteenth: factor = 1.0f / 4.0f * 1.5f; break;
            case DottedEighth: factor = 1.0f / 2.0f * 1.5f; break;
            case DottedQuarter: factor = 1.5f; break;
            case TripletEighth: factor = 1.0f / 3.0f; break;
            case TripletQuarter: factor = 2.0f / 3.0f; break;
            default: break;
        }

        return quarterSec * factor;
    }

    inline juce::String noteToString(Note note)
    {
        const char* names[] = {
            "1/64", "1/32", "1/16", "1/8", "1/4", "1/2", "1 Bar", "2 Bars",
            "1/16 Dotted", "1/8 Dotted", "1/4 Dotted", "1/8 Triplet", "1/4 Triplet"
        };

        return names[juce::jlimit(0, 12, static_cast<int>(note))];
    }
}
