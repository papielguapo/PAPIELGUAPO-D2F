#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
class Phobos67Processor final : public juce::AudioProcessor {
public:
 Phobos67Processor();
 const juce::String getName() const override{return "PAPIELGUAPO - D2F";}
 void prepareToPlay(double,int) override;void releaseResources() override{};void reset() override;
 void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
 void processBlockBypassed(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
 bool isBusesLayoutSupported(const BusesLayout&) const override;
 juce::AudioProcessorEditor* createEditor() override;bool hasEditor() const override{return true;}
 bool acceptsMidi() const override{return false;}bool producesMidi() const override{return false;}bool isMidiEffect() const override{return false;}
 double getTailLengthSeconds() const override{return .15;}int getNumPrograms() override{return 1;}int getCurrentProgram() override{return 0;}
 void setCurrentProgram(int) override{};const juce::String getProgramName(int) override{return "Default";}void changeProgramName(int,const juce::String&) override{}
 void getStateInformation(juce::MemoryBlock&) override;void setStateInformation(const void*,int) override;
 juce::AudioProcessorValueTreeState state;
 std::atomic<float> inPeak{-100},outPeak{-100};
 std::atomic<float> inK12{-100},outK12{-100};
 std::array<std::atomic<float>,2> inPeakLR{{-100.f,-100.f}},outPeakLR{{-100.f,-100.f}},inK12LR{{-100.f,-100.f}},outK12LR{{-100.f,-100.f}};
 static float inflate(float x,float curve,float effect,bool clip) noexcept;
private:
 friend struct DspProbeAccess;
 static juce::AudioProcessorValueTreeState::ParameterLayout layout();
 struct Biquad {std::array<float,5> c{1,0,0,0,0};float z1=0,z2=0;bool robust=false;std::array<double,5> dc{1,0,0,0,0};double dz1=0,dz2=0;
 float tick(float x) noexcept {if(robust){double y=dc[0]*x+dz1;dz1=dc[1]*x-dc[3]*y+dz2;dz2=dc[2]*x-dc[4]*y;return (float)y;}float y=c[0]*x+z1;z1=c[1]*x-c[3]*y+z2;z2=c[2]*x-c[4]*y;return y;}
 void clear(){z1=z2=0;dz1=dz2=0;}};
 struct SplitState {double low=0,high=0,midHigh=0,midLow=0;void clear(){low=high=midHigh=midLow=0;}};
 struct SplitFilter {double b=0,pole=0;double process(double x,double& state,bool highpass) const noexcept {const double y=b*x+state;state=(highpass?-b:b)*x+pole*y;return y;}};
 std::array<SplitState,2> splitState;std::array<bool,2> nativePolarity{};
 SplitFilter splitLow,splitHigh,splitMidHigh,splitMidLow;
 double splitMidGain=1;

 float processInflator(float,double,double,bool,int,bool) noexcept;
 static double inflateCore(double,double,double) noexcept;
 std::array<Biquad,2> air,sub;
 std::unique_ptr<juce::dsp::Oversampling<float>> os;
 juce::AudioBuffer<float> scratch,dry,control;juce::AudioBuffer<double> inflateControl;
 double effectCurrent=0,curveCurrent=.5,nativeAlpha=1.0/256;
 std::array<double,2> inputMeterLR{},outputMeterLR{};
 int capacity=0,delay=0,dryPos=0;double rate=48000;
 std::array<std::array<float,256>,2> dryDelay{};
 static constexpr double targetPower=0.03154786722400966; // Measurement reference only: K-12 zero (AES17).
 double inputMeter=0,outputMeter=0,detectorAlpha=0;
 std::array<juce::SmoothedValue<float>,8> smoothed;juce::SmoothedValue<float> fobosOutputStart,fobosInputStart;
 std::array<float,4> oldFilter{-1,-1,-1,-1};
 float value(const char* id) const noexcept{return state.getRawParameterValue(id)->load();}
 void updateFilters();void delayDry(juce::AudioBuffer<float>&);
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Phobos67Processor)
};
