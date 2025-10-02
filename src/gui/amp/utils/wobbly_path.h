#pragma once
#include "perlin.h"

inline juce::Path createWobblyPath(
    juce::Point<float> center, float radius, float noiseAmount,
    float noiseFrequency, float zOffset
)
{
    PerlinNoise perlin_noise;
    juce::Path path;
    const int numPoints = 200; // More points for a smoother curve

    for (int i = 0; i <= numPoints; ++i)
    {
        float angle = juce::MathConstants<float>::twoPi * (float)i / numPoints;

        // Use noise to modulate the radius
        // We use angle for x, a fixed value for y, and zOffset for z
        // to get a 1D slice of 3D noise that loops perfectly.
        float noiseX = (cos(angle) + 1.0f) * noiseFrequency;
        float noiseY = (sin(angle) + 1.0f) * noiseFrequency;

        double noiseValue =
            (perlin_noise.noise(noiseX, noiseY, zOffset) + 1.0) / 2.0;

        float modulatedRadius =
            radius *
            (1.0f - noiseAmount + (noiseAmount * 2.0f * (float)noiseValue));

        // Convert polar coordinates (angle, radius) to Cartesian (x, y)
        float x = center.x + modulatedRadius * cos(angle);
        float y = center.y + modulatedRadius * sin(angle);

        if (i == 0)
            path.startNewSubPath(x, y);
        else
            path.lineTo(x, y);
    }

    path.closeSubPath();
    return path;
}
