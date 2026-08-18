#include "preset_icon_buttons.h"
#include "BinaryData.h"

IconButton::IconButton(IconType type) : iconType(type)
{
    const char* data = nullptr;
    int dataSize = 0;

    switch (iconType)
    {
        case Folder:
            data = BinaryData::folderopen_svg;
            dataSize = BinaryData::folderopen_svgSize;
            break;
        case NewCollection:
            data = BinaryData::folderplus_svg;
            dataSize = BinaryData::folderplus_svgSize;
            break;
        case Save:
            data = BinaryData::save_svg;
            dataSize = BinaryData::save_svgSize;
            break;
        case Reload:
            data = BinaryData::rotateccw_svg;
            dataSize = BinaryData::rotateccw_svgSize;
            break;
        case Settings:
            data = BinaryData::settings_svg;
            dataSize = BinaryData::settings_svgSize;
            break;
    }

    if (auto svg = juce::XmlDocument::parse(
            juce::String::fromUTF8(data, dataSize)
        ))
    {
        icon = juce::Drawable::createFromSVG(*svg);
    }
}

void IconButton::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    juce::Colour iconColour =
        isHovered ? ColourCodes::white1 : ColourCodes::white0;

    if (icon == nullptr)
        return;

    icon->replaceColour(currentIconColour, iconColour);
    currentIconColour = iconColour;

    const float iconSize =
        juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    icon->drawWithin(
        g, bounds.withSizeKeepingCentre(iconSize, iconSize),
        juce::RectanglePlacement::centred, 1.0f
    );
}

void IconButton::mouseDown(const juce::MouseEvent&)
{
    if (onClick)
        onClick();
}

void IconButton::mouseEnter(const juce::MouseEvent&)
{
    isHovered = true;
    repaint();
}

void IconButton::mouseExit(const juce::MouseEvent&)
{
    isHovered = false;
    repaint();
}

PresetIconButtons::PresetIconButtons()
{
    loadSessionButton = std::make_unique<IconButton>(IconButton::Folder);
    loadSessionButton->onClick = [this]()
    {
        if (onLoadSessionClicked)
            onLoadSessionClicked();
    };
    addAndMakeVisible(loadSessionButton.get());

    newCollectionButton =
        std::make_unique<IconButton>(IconButton::NewCollection);
    newCollectionButton->onClick = [this]()
    {
        if (onNewCollectionClicked)
            onNewCollectionClicked();
    };
    addAndMakeVisible(newCollectionButton.get());

    savePresetButton = std::make_unique<IconButton>(IconButton::Save);
    savePresetButton->onClick = [this]()
    {
        if (onSavePresetClicked)
            onSavePresetClicked();
    };
    addAndMakeVisible(savePresetButton.get());

    reloadPresetButton = std::make_unique<IconButton>(IconButton::Reload);
    reloadPresetButton->onClick = [this]()
    {
        if (onReloadPresetClicked)
            onReloadPresetClicked();
    };
    addAndMakeVisible(reloadPresetButton.get());
}

PresetIconButtons::~PresetIconButtons()
{
}

void PresetIconButtons::resized()
{
    auto bounds = getLocalBounds();
    auto iconButtonSize = bounds.getWidth() / 4;

    loadSessionButton->setBounds(bounds.removeFromLeft(iconButtonSize));
    newCollectionButton->setBounds(bounds.removeFromLeft(iconButtonSize));
    savePresetButton->setBounds(bounds.removeFromLeft(iconButtonSize));
    reloadPresetButton->setBounds(bounds.removeFromLeft(iconButtonSize));
}

void PresetIconButtons::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::transparentBlack);
}
