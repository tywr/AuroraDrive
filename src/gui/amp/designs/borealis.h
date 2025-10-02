#include "../utils/voronoi.h"

inline void paintDesignBorealis(
    juce::Graphics& g, juce::Rectangle<float> bounds, juce::Colour c1,
    juce::Colour c2, const std::vector<juce::Point<float>>& sites,
    const std::vector<juce::Path>& cells
)
{
    juce::ColourGradient gradient = juce::ColourGradient(
        juce::Colours::blue, {0, 0}, juce::Colours::white, {0, 1.0f}, false
    );
    juce::Path clipPath;
    clipPath.addEllipse(bounds);
    juce::Graphics::ScopedSaveState state(g);
    g.reduceClipRegion(clipPath);

    const juce::Rectangle<float> templateBounds(0.0f, 0.0f, 1000.0f, 1000.0f);
    float scaleX = bounds.getWidth() / templateBounds.getWidth();
    float scaleY = bounds.getHeight() / templateBounds.getHeight();

    juce::AffineTransform transform =
        juce::AffineTransform::scale(scaleX, scaleY)
            .translated(bounds.getX(), bounds.getY());

    for (size_t i = 0; i < cells.size(); ++i)
    {
        if (i >= sites.size())
            continue;

        auto& sitePoint = sites[i];
        float proportion = sitePoint.y / templateBounds.getHeight();
        juce::Colour cellColour = gradient.getColourAtPosition(proportion);

        g.setColour(cellColour);
        g.fillPath(cells[i], transform);

        g.setColour(juce::Colours::white.withAlpha(0.15f));
        g.strokePath(cells[i], juce::PathStrokeType(1.5f), transform);

        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.strokePath(cells[i], juce::PathStrokeType(1.5f), transform);
    }
}
