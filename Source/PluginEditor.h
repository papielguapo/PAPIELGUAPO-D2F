#pragma once
#include "PluginProcessor.h"
class InnerKnob final : public juce::Slider {
public:bool hitTest(int x,int y) override {if(!getProperties().getWithDefault("concentric",false))return true;if(y>=96)return true;float dx=x-getWidth()*.5f,dy=y-getWidth()*.5f;return dx*dx+dy*dy<=37*37;}
};
class FrequencyRing final : public juce::Slider {
public:bool hitTest(int x,int y) override {float dx=x-getWidth()*.5f,dy=y-getHeight()*.5f;float r=std::sqrt(dx*dx+dy*dy);return r>=getWidth()*.35f && r<=getWidth()*.5f;}
};
class FET67LookAndFeel final : public juce::LookAndFeel_V4 {
public:juce::Slider::SliderLayout getSliderLayout(juce::Slider&) override;
void drawRotarySlider(juce::Graphics&,int,int,int,int,float,float,float,juce::Slider&) override;
void drawToggleButton(juce::Graphics&,juce::ToggleButton&,bool,bool) override;
};
class D2FFaceplate final : public juce::Component,private juce::Timer {
public:explicit D2FFaceplate(Phobos67Processor&);~D2FFaceplate() override;void paint(juce::Graphics&) override;void resized() override;
private:Phobos67Processor& p;FET67LookAndFeel look;juce::Component controlsLayer;std::array<InnerKnob,10> knobs;std::array<FrequencyRing,2> frequencies;std::array<juce::ToggleButton,8> buttons;
using SA=juce::AudioProcessorValueTreeState::SliderAttachment;using BA=juce::AudioProcessorValueTreeState::ButtonAttachment;
std::array<std::unique_ptr<SA>,10> sa;std::array<std::unique_ptr<SA>,2> frequencyAttachments;std::array<std::unique_ptr<BA>,8> ba;
juce::TooltipWindow tips{this,500};void timerCallback() override{repaint();}
};

class Phobos67Editor final : public juce::AudioProcessorEditor {
public:explicit Phobos67Editor(Phobos67Processor&);void resized() override;
private:D2FFaceplate faceplate;
};
