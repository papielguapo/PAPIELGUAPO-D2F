#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>
namespace {using AP=juce::AudioParameterFloat;using ID=juce::ParameterID;}
juce::AudioProcessorValueTreeState::ParameterLayout Phobos67Processor::layout(){
 juce::AudioProcessorValueTreeState::ParameterLayout l;
 auto f=[&](const char* id,const char* name,float lo,float hi,float step,float def){l.add(std::make_unique<AP>(ID{id,1},name,juce::NormalisableRange<float>(lo,hi,step),def));};
 f("input","Input",-24,24,.01f,0);f("even","Even Harmonics",0,100,1,0);f("odd","Odd Harmonics",0,100,1,0);
 f("air","Air Gain",0,10,1,0);f("sub","Sub Gain",0,10,1,0);f("mid","Mid Gain",-20,20,1,0);f("side","Side Gain",-20,20,1,0);
 f("effect","Inflate Effect",0,100,.01f,0);f("curve","Inflate Curve",-50,50,.01f,0);f("output","Output",-24,24,.01f,0);
 l.add(std::make_unique<juce::AudioParameterChoice>(ID{"airFreq",1},"Air Frequency",juce::StringArray{"20 kHz","30 kHz","40 kHz","50 kHz"},0));
 l.add(std::make_unique<juce::AudioParameterChoice>(ID{"subFreq",1},"Sub Frequency",juce::StringArray{"40 Hz","50 Hz","60 Hz","100 Hz"},2));
 l.add(std::make_unique<juce::AudioParameterBool>(ID{"fobosOn",1},"Meter Input / Output",true));
 l.add(std::make_unique<juce::AudioParameterBool>(ID{"inflateOn",1},"Meter K-12 / K-14",true));
 l.add(std::make_unique<juce::AudioParameterBool>(ID{"clip",1},"Clip 0 dB",true));
 l.add(std::make_unique<juce::AudioParameterBool>(ID{"power",1},"Power",true));
 l.add(std::make_unique<juce::AudioParameterBool>(ID{"bandSplit",1},"Inflator Band Split",false));
 l.add(std::make_unique<juce::AudioParameterBool>(ID{"reverseOrder",1},"Order B: Inflate before Color",false));
 l.add(std::make_unique<juce::AudioParameterBool>(ID{"mono",1},"Monitor Mono",false));
 l.add(std::make_unique<juce::AudioParameterBool>(ID{"delta",1},"Monitor Delta",false));
 return l;
}
Phobos67Processor::Phobos67Processor():AudioProcessor(BusesProperties().withInput("Input",juce::AudioChannelSet::stereo(),true).withOutput("Output",juce::AudioChannelSet::stereo(),true)),state(*this,nullptr,"PHOBOS67",layout()){}
bool Phobos67Processor::isBusesLayoutSupported(const BusesLayout& l) const {return l.getMainInputChannelSet()==l.getMainOutputChannelSet()&&(l.getMainInputChannelSet()==juce::AudioChannelSet::mono()||l.getMainInputChannelSet()==juce::AudioChannelSet::stereo());}
void Phobos67Processor::prepareToPlay(double sr,int bs){rate=sr;capacity=juce::jmax(32,bs);scratch.setSize(2,capacity);dry.setSize(2,capacity);control.setSize(4,capacity);inflateControl.setSize(2,capacity);detectorAlpha=1-std::exp(-1/(rate*.6));
 os=std::make_unique<juce::dsp::Oversampling<float>>(2,2,juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple,false,true);os->initProcessing((size_t)capacity);delay=juce::roundToInt(os->getLatencyInSamples());setLatencySamples(delay);
 // Canonical coefficients keep rounding deterministic at standard sample rates.
 struct Coefficients {double rate,lowB,lowPole,highB,highPole,midHighB,midHighPole,midLowB,midLowPole,attenuation;};
 static constexpr Coefficients canonical[]{
  {22050.0,0.03307608755085591,0.9338478248982881,0.7375047612300502,0.4750095224601002,0.966923912449144,0.9338478248982881,0.26249523876994996,0.4750095224601002,0.9038907407185084},
  {24000.0,0.030468747091253887,0.9390625058174921,0.754762724747214,0.509525449494428,0.969531252908746,0.9390625058174922,0.24523727525278605,0.5095254494944279,0.9032798983634885},
  {44100.0,0.016811316150682133,0.9663773676986357,0.8527645468945084,0.7055290937890168,0.9831886838493179,0.9663773676986358,0.14723545310549152,0.7055290937890168,0.9009666241496598},
  {48000.0,0.015466291403103396,0.9690674171937933,0.8632712640026802,0.7265425280053603,0.9845337085968967,0.9690674171937933,0.13672873599731983,0.7265425280053603,0.9008156654057937},
  {88200.0,0.008476297847396978,0.9830474043052061,0.921069534665058,0.842139069330116,0.9915237021526031,0.9830474043052061,0.07893046533494198,0.8421390693301161,0.9002412811373851},
  {96000.0,0.007792936291951569,0.9844141274160969,0.9270403427317333,0.8540806854634664,0.9922070637080485,0.9844141274160969,0.07295965726826684,0.8540806854634665,0.9002036493759147},
  {176400.0,0.004256109851111122,0.9914877802977778,0.9589853421750296,0.9179706843500591,0.995743890148889,0.9914877802977778,0.04101465782497046,0.9179706843500592,0.9000602969135534},
  {192000.0,0.003911649911246553,0.9921767001775069,0.9621952458291035,0.9243904916582069,0.9960883500887534,0.9921767001775068,0.03780475417089656,0.924390491658207,0.9000508956943478},
  {352800.0,0.0021325834814465345,0.995734833037107,0.9790726808815688,0.9581453617631377,0.9978674165185535,0.995734833037107,0.020927319118431206,0.9581453617631377,0.9000150727686558},
  {384000.0,0.001959650162753165,0.9960806996744936,0.9807407257976642,0.9614814515953284,0.9980403498372468,0.9960806996744936,0.019259274202335794,0.9614814515953284,0.9000127228835515},
 };
 const double lowK=std::tan(3.1415926535898*240.0/rate),highK=std::tan(3.1415926535898*2400.0/rate);
 splitLow={lowK/(1+lowK),(1-lowK)/(1+lowK)};splitHigh={1/(1+highK),(1-highK)/(1+highK)};
 splitMidHigh={1/(1+lowK),(1-lowK)/(1+lowK)};splitMidLow={highK/(1+highK),(1-highK)/(1+highK)};splitMidGain=1/(1-lowK/highK);
 for(const auto& c:canonical)if(rate==c.rate){splitLow={c.lowB,c.lowPole};splitHigh={c.highB,c.highPole};splitMidHigh={c.midHighB,c.midHighPole};splitMidLow={c.midLowB,c.midLowPole};splitMidGain=1/c.attenuation;break;}

 const float defaults[]{1,0,0,0,0,0,.5f,1};for(size_t i=0;i<smoothed.size();i++){smoothed[i].reset(rate,.05);smoothed[i].setCurrentAndTargetValue(defaults[i]);}
 fobosInputStart.reset(rate,.05);fobosInputStart.setCurrentAndTargetValue(0);fobosInputStart.setTargetValue(1);fobosOutputStart.reset(rate,.05);fobosOutputStart.setCurrentAndTargetValue(0);fobosOutputStart.setTargetValue(1);effectCurrent=value("effect")/100.f;curveCurrent=(value("curve")+50)/100.f;int rateFactor=1;while(rate>48000.0*rateFactor)rateFactor*=2;nativeAlpha=1.0/(256*rateFactor);reset();oldFilter.fill(-1);updateFilters();}
void Phobos67Processor::reset(){nativePolarity.fill(false);inputMeterLR.fill(0);outputMeterLR.fill(0);for(int ch=0;ch<2;ch++){inPeakLR[ch].store(-100);outPeakLR[ch].store(-100);inK12LR[ch].store(-100);outK12LR[ch].store(-100);}for(auto& s:splitState)s.clear();if(os)os->reset();for(auto& b:air)b.clear();for(auto& b:sub)b.clear();for(auto& d:dryDelay)d.fill(0);dryPos=0;inputMeter=outputMeter=0;inK12.store(-100);outK12.store(-100);}
void Phobos67Processor::updateFilters(){std::array<float,4> v{value("air"),value("airFreq"),value("sub"),value("subFreq")};if(v==oldFilter)return;oldFilter=v;
 const float sf[]{40,50,60,100};float sr=(float)(rate*4);
 auto ac=juce::dsp::IIR::ArrayCoefficients<float>::makeHighShelf(sr,juce::jmin(sr*.49f,50000.f+5000.f*v[1]),.1f,juce::Decibels::decibelsToGain(v[0]*2));
 auto sc=juce::dsp::IIR::ArrayCoefficients<float>::makeLowShelf(sr,sf[juce::jlimit(0,3,(int)v[3])],.5f,juce::Decibels::decibelsToGain(v[2]));
 const float ai=1.f/ac[3],si=1.f/sc[3];
 for(auto& b:air)for(int i=0;i<5;i++)b.c[(size_t)i]=ac[(size_t)(i<3?i:i+1)]*ai;
 for(auto& b:sub)for(int i=0;i<5;i++)b.c[(size_t)i]=sc[(size_t)(i<3?i:i+1)]*si;
 // Float coefficient rounding can move a very-low shelf pole outside the unit circle.
 // Preserve the measured float path except when the rounded denominator is unstable.
 for(auto& b:sub){bool useDouble=1.0+(double)b.c[3]+b.c[4]<=0||1.0-(double)b.c[3]+b.c[4]<=0||1.0-(double)b.c[4]<=0;
  if(useDouble!=b.robust){if(useDouble){b.dz1=b.z1;b.dz2=b.z2;}else{b.z1=(float)b.dz1;b.z2=(float)b.dz2;}}b.robust=useDouble;
  if(useDouble){auto d=juce::dsp::IIR::ArrayCoefficients<double>::makeLowShelf(rate*4,(double)sf[juce::jlimit(0,3,(int)v[3])],.5,juce::Decibels::decibelsToGain((double)v[2]));for(int i=0;i<5;i++)b.dc[(size_t)i]=d[(size_t)(i<3?i:i+1)]/d[3];}
 }
}
double Phobos67Processor::inflateCore(double z,double c,double e) noexcept{
 z=juce::jlimit(-1.9999998,1.9999998,z);const double a=std::abs(z);
 double y=a<=1?a*((1+c)+a*((.75-1.5*c-.25*c*c)+a*((-.5+.5*c*c)+a*(-.25*(1-c)*(1-c))))):1-(a-1)*(a-1);
 return (1-e)*z+e*std::copysign(y,z)*.9999995529651642;
}
float Phobos67Processor::inflate(float x,float c,float e,bool clip) noexcept{
 return (float)inflateCore(juce::jlimit(clip?-1.0:-1.9999998,clip?1.0:1.9999998,(double)x),c,e);
}
float Phobos67Processor::processInflator(float x,double c,double e,bool clip,int ch,bool split) noexcept{
 const double input=x==0.f?(nativePolarity[(size_t)ch]?-7e-15:7e-15):(double)x;nativePolarity[(size_t)ch]=!nativePolarity[(size_t)ch];
 const double z=juce::jlimit(clip?-1.0:-1.9999998,clip?1.0:1.9999998,input);
 auto& s=splitState[(size_t)ch];
 const double low=splitLow.process(z,s.low,false),high=splitHigh.process(z,s.high,true);
 const double midHigh=splitMidHigh.process(z,s.midHigh,true);
 const double mid=splitMidLow.process(midHigh,s.midLow,false);
 if(!split)return (float)inflateCore(z,c,e);
 return (float)((1-e)*z+e*(inflateCore(low,c,1)+inflateCore(mid,c,1)/splitMidGain+inflateCore(high,c,1)));
}
void Phobos67Processor::delayDry(juce::AudioBuffer<float>& b){for(int i=0;i<b.getNumSamples();i++){for(int c=0;c<b.getNumChannels();c++){auto& ring=dryDelay[(size_t)c];float x=b.getSample(c,i);b.setSample(c,i,ring[(size_t)dryPos]);ring[(size_t)dryPos]=x;}dryPos=(dryPos+1)%juce::jmax(1,delay);}}
void Phobos67Processor::processBlockBypassed(juce::AudioBuffer<float>& b,juce::MidiBuffer&){
 juce::ScopedNoDenormals nd;delayDry(b);float peak=0;
 for(int j=0;j<b.getNumSamples();j++){double energy=0;for(int ch=0;ch<2;ch++){float x=b.getSample(juce::jmin(ch,b.getNumChannels()-1),j);inputMeterLR[ch]+=detectorAlpha*((double)x*x-inputMeterLR[ch]);outputMeterLR[ch]=inputMeterLR[ch];if(ch<b.getNumChannels()){peak=juce::jmax(peak,std::abs(x));energy+=(double)x*x;}}energy/=b.getNumChannels();inputMeter+=detectorAlpha*(energy-inputMeter);outputMeter+=detectorAlpha*(energy-outputMeter);}
 float peakDb=juce::Decibels::gainToDecibels(peak,-100.f);inPeak.store(peakDb);outPeak.store(peakDb);inK12.store((float)(10*std::log10(juce::jmax(1e-20,inputMeter)/targetPower)));outK12.store((float)(10*std::log10(juce::jmax(1e-20,outputMeter)/targetPower)));
 for(int ch=0;ch<2;ch++){float db=juce::Decibels::gainToDecibels(b.getMagnitude(juce::jmin(ch,b.getNumChannels()-1),0,b.getNumSamples()),-100.f);inPeakLR[ch].store(db);outPeakLR[ch].store(db);inK12LR[ch].store((float)(10*std::log10(juce::jmax(1e-20,inputMeterLR[ch])/targetPower)));outK12LR[ch].store((float)(10*std::log10(juce::jmax(1e-20,outputMeterLR[ch])/targetPower)));}
}
void Phobos67Processor::processBlock(juce::AudioBuffer<float>& b,juce::MidiBuffer&){juce::ScopedNoDenormals nd;if(!os){b.clear();return;}
 float normalizedPeak=0,peak=0;float inPeaks[2]{},outPeaks[2]{};
 updateFilters();const bool clip=value("clip")>.5f,power=value("power")>.5f,split=value("bandSplit")>.5f,reverse=value("reverseOrder")>.5f,mono=value("mono")>.5f,delta=value("delta")>.5f;
 const float targets[]{juce::Decibels::decibelsToGain(value("input")),juce::roundToInt(value("even"))*.01f,juce::roundToInt(value("odd"))*.01f,juce::Decibels::decibelsToGain(value("mid")),juce::Decibels::decibelsToGain(value("side")),value("effect")/100.f,(value("curve")+50)/100.f,juce::Decibels::decibelsToGain(value("output"))};for(size_t k=0;k<smoothed.size();k++)smoothed[k].setTargetValue(targets[k]);
 for(int offset=0;offset<b.getNumSamples();offset+=capacity){int n=juce::jmin(capacity,b.getNumSamples()-offset);scratch.setSize(2,n,false,false,true);dry.setSize(b.getNumChannels(),n,false,false,true);
  for(int ch=0;ch<b.getNumChannels();ch++){scratch.copyFrom(ch,0,b,ch,offset,n);dry.copyFrom(ch,0,b,ch,offset,n);}if(b.getNumChannels()==1)scratch.copyFrom(1,0,scratch,0,0,n);delayDry(dry);
  for(int j=0;j<n;j++){
   const float gain=smoothed[0].getNextValue(),tmcInputStart=fobosInputStart.getNextValue();control.setSample(3,j,gain);control.setSample(2,j,smoothed[7].getNextValue());control.setSample(1,j,fobosOutputStart.getNextValue());
   inflateControl.setSample(0,j,effectCurrent);inflateControl.setSample(1,j,curveCurrent);effectCurrent+=(targets[5]-effectCurrent)*nativeAlpha;curveCurrent+=(targets[6]-curveCurrent)*nativeAlpha;
   for(int ch=0;ch<2;ch++){float x=scratch.getSample(ch,j)*gain;if(reverse)x=processInflator(x,inflateControl.getSample(1,j),inflateControl.getSample(0,j),clip,ch,split);x*=tmcInputStart;scratch.setSample(ch,j,x);}
  }
  juce::dsp::AudioBlock<float> block(scratch);auto up=os->processSamplesUp(block);
  for(size_t j=0;j<up.getNumSamples();j++){
   float v[5];for(size_t k=1;k<5;k++)v[k]=smoothed[k].getNextValue();
   float l=up.getChannelPointer(0)[j],r=up.getChannelPointer(1)[j];
   for(int ch=0;ch<2;ch++){float& x=ch==0?l:r;x=(x+(2*v[1])*(x*x)+(2*v[2])*(x*x*x))*(1.f/(1+1.2f*(v[1]+v[2])));}
   float mid=(l+r)*.5f*v[3],side=(l-r)*.5f*v[4];l=mid+side;r=mid-side;
   for(int ch=0;ch<2;ch++){float& x=ch==0?l:r;x=air[(size_t)ch].tick(x);x=sub[(size_t)ch].tick(x);}up.getChannelPointer(0)[j]=l;up.getChannelPointer(1)[j]=r;
  }
  for(auto& f:air){JUCE_SNAP_TO_ZERO(f.z1);JUCE_SNAP_TO_ZERO(f.z2);}for(auto& f:sub){JUCE_SNAP_TO_ZERO(f.z1);JUCE_SNAP_TO_ZERO(f.z2);}os->processSamplesDown(block);
  for(int j=0;j<n;j++){
   float processed[2]{},original[2]{};double inEnergy=0,outEnergy=0;
   for(int ch=0;ch<2;ch++){
    const int channel=juce::jmin(ch,b.getNumChannels()-1);original[ch]=dry.getSample(channel,j);const float input=original[ch]*control.getSample(3,j);inPeaks[ch]=juce::jmax(inPeaks[ch],std::abs(input));inputMeterLR[ch]+=detectorAlpha*((double)input*input-inputMeterLR[ch]);if(ch<b.getNumChannels())inEnergy+=(double)input*input;
    float x=scratch.getSample(ch,j)*control.getSample(1,j);if(!reverse)x=processInflator(x,inflateControl.getSample(1,j),inflateControl.getSample(0,j),clip,ch,split);processed[ch]=x*control.getSample(2,j);
   }
   if(mono&&power){processed[0]=processed[1]=(processed[0]+processed[1])*.5f;original[0]=original[1]=(original[0]+original[1])*.5f;}
   for(int ch=0;ch<2;ch++){float wet=processed[ch];if(power&&clip)wet=juce::jlimit(-1.f,1.f,wet);float x=power?(delta?wet-original[ch]:wet):dry.getSample(juce::jmin(ch,b.getNumChannels()-1),j);if(power&&clip)x=juce::jlimit(-1.f,1.f,x);outPeaks[ch]=juce::jmax(outPeaks[ch],std::abs(x));outputMeterLR[ch]+=detectorAlpha*((double)x*x-outputMeterLR[ch]);if(ch<b.getNumChannels()){b.setSample(ch,offset+j,x);outEnergy+=(double)x*x;}}
   inputMeter+=detectorAlpha*(inEnergy/b.getNumChannels()-inputMeter);outputMeter+=detectorAlpha*(outEnergy/b.getNumChannels()-outputMeter);
  }
 }
 normalizedPeak=juce::jmax(inPeaks[0],inPeaks[1]);peak=juce::jmax(outPeaks[0],outPeaks[1]);inPeak.store(juce::Decibels::gainToDecibels(normalizedPeak,-100.f));outPeak.store(juce::Decibels::gainToDecibels(peak,-100.f));inK12.store((float)(10*std::log10(juce::jmax(1e-20,inputMeter)/targetPower)));outK12.store((float)(10*std::log10(juce::jmax(1e-20,outputMeter)/targetPower)));
 for(int ch=0;ch<2;ch++){inPeakLR[ch].store(juce::Decibels::gainToDecibels(inPeaks[ch],-100.f));outPeakLR[ch].store(juce::Decibels::gainToDecibels(outPeaks[ch],-100.f));inK12LR[ch].store((float)(10*std::log10(juce::jmax(1e-20,inputMeterLR[ch])/targetPower)));outK12LR[ch].store((float)(10*std::log10(juce::jmax(1e-20,outputMeterLR[ch])/targetPower)));}
}
juce::AudioProcessorEditor* Phobos67Processor::createEditor(){return new Phobos67Editor(*this);}
void Phobos67Processor::getStateInformation(juce::MemoryBlock& b){auto xml=state.copyState().createXml();copyXmlToBinary(*xml,b);}
void Phobos67Processor::setStateInformation(const void* data,int size){if(auto xml=getXmlFromBinary(data,size))if(xml->hasTagName("PHOBOS67")){auto s=juce::ValueTree::fromXml(*xml);for(auto child:s){auto id=child.getProperty("id").toString();if(auto* p=state.getParameter(id)){auto v=(float)child.getProperty("value");if(std::isfinite(v))p->setValueNotifyingHost(p->convertTo0to1(juce::jlimit(p->getNormalisableRange().start,p->getNormalisableRange().end,v)));}}}}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new Phobos67Processor;}

