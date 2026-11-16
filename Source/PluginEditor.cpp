#include "PluginEditor.h"
#include <charconv>
#include <cmath>

namespace
{
constexpr int knobCentres[]{225,328,328,1105,955,1240,1240,1500,1360,1360};
juce::Colour colour(juce::uint32 hex) { return juce::Colour(hex); }
juce::Font font(const juce::String& name,float size,int style)
{
#if JUCE_MAJOR_VERSION >= 8
    return juce::Font(juce::FontOptions(name,size,style));
#else
    return juce::Font(name,size,style);
#endif
}
void text(juce::Graphics& g, const juce::String& s, float x, float y, float w, float h, float size = 10, bool bold = true)
{
    g.setFont(font("Arial", size, bold ? juce::Font::bold : juce::Font::plain));
    g.drawText(s, juce::Rectangle<float>(x,y,w,h), juce::Justification::centred, false);
}
void screw(juce::Graphics& g, float x, float y)
{
    g.setColour(juce::Colours::white.withAlpha(0.42f)); g.fillEllipse(x-15,y-14,31,31);
    g.setGradientFill(juce::ColourGradient(colour(0xff080b0c),x-8,y-12,colour(0xff353b3c),x+9,y+12,false));
    g.fillEllipse(x-15,y-15,30,30);
    g.setGradientFill(juce::ColourGradient(colour(0xff191f20),x-3,y-6,colour(0xff020404),x+4,y+5,true));
    g.fillEllipse(x-6,y-6,12,12);
}
void lamp(juce::Graphics& g, juce::Rectangle<float> r, bool on)
{
    const auto c=r.getCentre();
    if(on) { g.setGradientFill(juce::ColourGradient(colour(0xff9b26ff).withAlpha(0.35f),c.x,c.y,juce::Colours::transparentBlack,c.x+r.getWidth()*0.85f,c.y,true));g.fillEllipse(r.expanded(r.getWidth()*0.4f)); }
    g.setColour(juce::Colours::black.withAlpha(0.3f));g.fillEllipse(r.translated(2,3));
    g.setGradientFill(juce::ColourGradient(colour(0xff73349d),r.getX(),r.getY(),colour(0xff260438),r.getRight(),r.getBottom(),false));g.fillEllipse(r);
    auto inner=r.reduced(5);
    if(on) { auto jewel=juce::ColourGradient(colour(0xffdf9aff),c.x-10,c.y-13,colour(0xff490078),c.x+24,c.y+20,true);jewel.addColour(0.36,colour(0xffb839ff));jewel.addColour(0.74,colour(0xff7900ce));g.setGradientFill(jewel);g.fillEllipse(inner);g.setColour(colour(0xffdd8fff).withAlpha(0.23f));g.drawEllipse(inner.reduced(1),2); }
    else { g.setColour(colour(0xff240046));g.fillEllipse(inner); }
    g.setColour(juce::Colours::white.withAlpha(on?0.83f:0.12f));g.fillEllipse(r.getX()+13,r.getY()+12,r.getWidth()*0.30f,r.getHeight()*0.19f);
    if(on){g.setColour(colour(0xffdc8fff).withAlpha(0.36f));g.fillEllipse(c.x+4,c.y+11,13,7);}
}
}

juce::Slider::SliderLayout FET67LookAndFeel::getSliderLayout(juce::Slider& s){
 if(s.getProperties().getWithDefault("smallStack",false)){juce::Slider::SliderLayout l;l.sliderBounds={0,0,56,56};l.textBoxBounds={55,19,48,18};return l;}
 if(s.getProperties().getWithDefault("concentric",false)){juce::Slider::SliderLayout l;l.sliderBounds={2,2,80,80};l.textBoxBounds={0,127,84,22};return l;}return juce::LookAndFeel_V4::getSliderLayout(s);
}
void FET67LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                              float pos, float start, float end, juce::Slider& slider)
{
    if(slider.getProperties().getWithDefault("frequencyRing",false)){
     float cx=x+width*.5f,cy=y+height*.5f,r=juce::jmin(width,height)*.47f;
     juce::Path ring;ring.setUsingNonZeroWinding(false);ring.addEllipse(cx-r,cy-r,r*2,r*2);ring.addEllipse(cx-36,cy-36,72,72);
     auto resin=juce::ColourGradient(colour(0xffaf40ff),cx-r*.55f,cy-r*.75f,colour(0xff200034),cx+r*.8f,cy+r*.9f,false);resin.addColour(.34,colour(0xff7e12d5));resin.addColour(.65,colour(0xff51008e));g.setGradientFill(resin);g.fillPath(ring);
     g.setColour(colour(0xffdd9cff).withAlpha(.8f));g.drawEllipse(cx-r+1,cy-r+1,r*2-2,r*2-2,1);
     g.setColour(colour(0xff26003f));g.drawEllipse(cx-36,cy-36,72,72,2);
     for(int i=0;i<28;i++){float a=i*juce::MathConstants<float>::twoPi/28;g.setColour(colour(i%2?0xffc45cff:0xff25003e).withAlpha(.6f));g.drawLine(cx+std::sin(a)*(r-7),cy-std::cos(a)*(r-7),cx+std::sin(a)*(r-2),cy-std::cos(a)*(r-2),1.3f);}
     float a=start+pos*(end-start);g.setColour(colour(0xfff8efff));g.drawLine(cx+std::sin(a)*(r-8),cy-std::cos(a)*(r-8),cx+std::sin(a)*(r-2),cy-std::cos(a)*(r-2),2.6f);return;
    }
    const float r = static_cast<float>(juce::jmin(width, height)) * 0.46f;
    const float cx = static_cast<float>(x) + static_cast<float>(width) * 0.5f;
    const float cy = static_cast<float>(y) + static_cast<float>(height) * 0.5f;
    g.setColour(juce::Colours::black.withAlpha(0.38f));
    g.fillEllipse(cx-r+2,cy-r+5,r*2+2,r*2+2);
    g.setColour(colour(0xff24202c));g.fillEllipse(cx-r-1,cy-r-1,r*2+2,r*2+2);
    auto resin=juce::ColourGradient(colour(0xffaf40ff),cx-r*0.55f,cy-r*0.75f,
                                   colour(0xff200034),cx+r*0.8f,cy+r*0.9f,false);
    resin.addColour(0.34,colour(0xff7e12d5));resin.addColour(0.65,colour(0xff51008e));
    g.setGradientFill(resin);g.fillEllipse(cx-r,cy-r,r*2,r*2);
    g.setColour(colour(0xffdd9cff).withAlpha(0.8f));g.drawEllipse(cx-r+1.4f,cy-r+1.4f,r*2-2.8f,r*2-2.8f,1.1f);
    g.setColour(colour(0xff26003f).withAlpha(0.75f));g.drawEllipse(cx-r+4,cy-r+4,r*2-8,r*2-8,1.4f);
    juce::Graphics::ScopedSaveState save(g);
    g.addTransform(juce::AffineTransform::rotation(start+pos*(end-start)).translated(cx,cy));
    // Narrow concave channels follow the fin; the stationary resin skirt catches the light.
    for(float side : {-1.0f,1.0f})
    {
        juce::Path flute;
        flute.startNewSubPath(side*r*0.25f,-r*0.88f);
        flute.cubicTo(side*r*0.73f,-r*0.63f,side*r*0.77f,r*0.46f,side*r*0.30f,r*0.85f);
        flute.cubicTo(side*r*0.47f,r*0.25f,side*r*0.47f,-r*0.32f,side*r*0.25f,-r*0.88f);
        flute.closeSubPath();g.setColour(colour(0xff25003e));g.fillPath(flute);
        juce::Path rib;rib.startNewSubPath(side*r*0.55f,-r*0.67f);
        rib.cubicTo(side*r*0.77f,-r*0.27f,side*r*0.73f,r*0.32f,side*r*0.48f,r*0.67f);
        g.setColour(colour(0xffc45cff).withAlpha(side<0?0.56f:0.22f));g.strokePath(rib,juce::PathStrokeType(1.0f));
    }
    juce::Path fin;
    fin.startNewSubPath(-r*0.07f,-r*0.94f);
    fin.cubicTo(r*0.12f,-r*0.97f,r*0.18f,-r*0.70f,r*0.19f,-r*0.25f);
    fin.lineTo(r*0.21f,r*0.58f);
    fin.quadraticTo(r*0.24f,r*0.86f,r*0.04f,r*0.91f);
    fin.quadraticTo(-r*0.18f,r*0.94f,-r*0.20f,r*0.68f);
    fin.lineTo(-r*0.19f,-r*0.60f);
    fin.quadraticTo(-r*0.19f,-r*0.89f,-r*0.07f,-r*0.94f);fin.closeSubPath();
    auto ridge=juce::ColourGradient(colour(0xffe3a2ff),-r*0.20f,0,colour(0xff3c0065),r*0.23f,0,false);
    ridge.addColour(0.23,colour(0xffb14cff));ridge.addColour(0.40,colour(0xff7b13c0));
    ridge.addColour(0.63,colour(0xff6500a8));g.setGradientFill(ridge);g.fillPath(fin);
    g.setColour(colour(0xffcc82ff).withAlpha(0.72f));g.strokePath(fin,juce::PathStrokeType(0.8f));
    g.setColour(colour(0xfff8efff));g.fillRoundedRectangle(-r*0.09f,-r*0.87f,r*0.075f,r*0.49f,0.7f);
    if(slider.hasKeyboardFocus(true)) {g.setColour(colour(0xffefd4ff));g.drawEllipse(-r-3,-r-3,r*2+6,r*2+6,1);}
}

void FET67LookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& b, bool hover, bool down)
{
    if(b.getProperties().getWithDefault("clipCompact",false)){
     const bool on=b.getToggleState();g.setColour(colour(0xff08070b));g.fillEllipse(0,8,8,8);g.setGradientFill(juce::ColourGradient(colour(on?0xffdf9aff:0xff382046),2,9,colour(on?0xff7900ce:0xff190523),6,15,true));g.fillEllipse(1,9,6,6);
     g.setColour(colour(0xff292330));text(g,"ON",10,6,17,12,8);text(g,"OFF",59,6,17,12,8);
     float cx=43,cy=12,tip=on?cx-5:cx+5;g.setGradientFill(juce::ColourGradient(colour(0xff555b5e),cx-7,cy-8,colour(0xff020506),cx+8,cy+8,false));g.fillEllipse(cx-9,cy-9,18,18);g.setColour(colour(0xff08090b));g.fillRoundedRectangle(juce::jmin(cx,tip)-4,cy-5,std::abs(tip-cx)+8,10,4);g.setGradientFill(juce::ColourGradient(colour(0xff515a60),tip-2,cy-4,colour(0xff101416),tip+3,cy+4,false));g.fillEllipse(tip-5,cy-5,10,10);if(hover||down){g.setColour(colour(0xffc45cff));g.drawEllipse(cx-10,cy-10,20,20,1);}return;
    }
    if(b.getProperties().getWithDefault("miniLed",false)){auto r=b.getLocalBounds().toFloat().reduced(2);g.setColour(colour(0xff060708));g.fillEllipse(r);r=r.reduced(2);auto c=r.getCentre();g.setGradientFill(juce::ColourGradient(colour(b.getToggleState()?0xffdf9aff:0xff382046),c.x-2,c.y-3,colour(b.getToggleState()?0xff7900ce:0xff190523),c.x+4,c.y+5,true));g.fillEllipse(r);if(b.getToggleState()){g.setColour(colour(0xfff0ccff));g.fillEllipse(r.getX()+2,r.getY()+1,3,2);}if(hover||down){g.setColour(colour(0xffe4c3ff));g.drawEllipse(r.expanded(2),1);}return;}
    const auto c=b.getLocalBounds().toFloat().getCentre();
    const bool right=b.getToggleState()!=static_cast<bool>(b.getProperties().getWithDefault("reverse",false));
    const float bx=c.x+3, tip=bx+(right?12.0f:-12.0f);
    g.setColour(juce::Colours::black.withAlpha(0.28f));g.fillEllipse(bx-12,c.y-11,27,28);
    g.setGradientFill(juce::ColourGradient(colour(0xff555b5e),bx-8,c.y-12,colour(0xff020506),bx+9,c.y+13,false));
    g.fillEllipse(bx-13,c.y-13,26,26);
    g.setColour(colour(0xff090c0d));g.fillEllipse(bx-10,c.y-10,20,20);
    g.setGradientFill(juce::ColourGradient(colour(0xff4c555b),tip,c.y-7,colour(0xff080b0d),tip,c.y+7,false));
    g.fillRoundedRectangle(juce::jmin(tip,bx)-5,c.y-7,std::abs(tip-bx)+10,14,6);
    g.setColour(colour(0xff030607));g.fillEllipse(tip-8,c.y-8,16,16);
    g.setGradientFill(juce::ColourGradient(colour(0xff515a60),tip-3,c.y-5,colour(0xff101416),tip+4,c.y+5,false));
    g.fillEllipse(tip-5,c.y-5.5f,10,11);
    if(hover||down||b.hasKeyboardFocus(true)){g.setColour(colour(0xffb84dff).withAlpha(0.65f));g.drawEllipse(bx-15,c.y-15,30,30,1);}
}

D2FFaceplate::D2FFaceplate(Phobos67Processor& processor):p(processor){
 setLookAndFeel(&look);setSize(1634,252);addAndMakeVisible(controlsLayer);controlsLayer.setName("controls");controlsLayer.setInterceptsMouseClicks(false,true);
 const char* ids[]{"input","even","odd","air","sub","effect","curve","output","mid","side"};
 const char* units[]{" dB"," %"," %"," dB"," dB"," %",""," dB"," dB"," dB"};
 for(size_t i=0;i<knobs.size();i++){auto& k=knobs[i];controlsLayer.addAndMakeVisible(k);k.setName(ids[i]);k.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);k.setTextBoxStyle(juce::Slider::TextBoxBelow,false,92,22);k.setRotaryParameters(juce::MathConstants<float>::pi*7/6,juce::MathConstants<float>::pi*17/6,true);k.setTextValueSuffix(units[i]);k.setColour(juce::Slider::textBoxTextColourId,colour(0xff24202c));k.setColour(juce::Slider::textBoxBackgroundColourId,colour(0xffe2e4e6));k.setColour(juce::Slider::textBoxOutlineColourId,colour(0xffb7b9bf));k.setNumDecimalPlacesToDisplay(i==3||i==4?0:1);k.setDoubleClickReturnValue(true,0);sa[i]=std::make_unique<SA>(p.state,ids[i],k);}
 const char* fids[]{"airFreq","subFreq"};
 for(size_t i=0;i<2;i++){auto& f=frequencies[i];controlsLayer.addAndMakeVisible(f);f.setName(fids[i]);f.setTooltip(i==0?"Air outer ring: 20 / 30 / 40 / 50 kHz":"Sub outer ring: 40 / 50 / 60 / 100 Hz");f.getProperties().set("frequencyRing",true);f.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);f.setTextBoxStyle(juce::Slider::NoTextBox,false,0,0);f.setRotaryParameters(juce::MathConstants<float>::pi*7/6,juce::MathConstants<float>::pi*17/6,true);f.setMouseDragSensitivity(140);frequencyAttachments[i]=std::make_unique<SA>(p.state,fids[i],f);f.toBack();knobs[i+3].getProperties().set("concentric",true);knobs[i+3].resized();knobs[i+3].setTooltip(i==0?"Air inner knob: gain; outer ring: frequency":"Sub inner knob: gain; outer ring: frequency");}
 const char* bids[]{"power","fobosOn","inflateOn","clip","bandSplit","reverseOrder","mono","delta"};
 const char* btips[]{"Power: latency-aligned bypass","Meter IN / OUT: signal after Input gain or final output","Lower L/R meter: K-12 / K-14 RMS. Upper meter always shows dBFS peaks","Clip waveshaper input and final output at 0 dBFS, after Output gain","Inflator Band Split: OFF / ON","Order A: Color then Inflate; B: Inflate then Color. Input first; Output and final Clip last","Mono / Stereo: mono sums L and R equally at the output","Delta: hear processed minus latency-aligned original input"};
 for(size_t i=0;i<8;i++){controlsLayer.addAndMakeVisible(buttons[i]);buttons[i].setName(bids[i]);buttons[i].setTooltip(btips[i]);buttons[i].setClickingTogglesState(true);ba[i]=std::make_unique<BA>(p.state,bids[i],buttons[i]);}
 for(int index:{1,2,5,6,8,9}){knobs[(size_t)index].getProperties().set("smallStack",true);knobs[(size_t)index].resized();}
 buttons[2].getProperties().set("reverse",true);buttons[4].getProperties().set("reverse",true);buttons[7].getProperties().set("reverse",true);buttons[3].getProperties().set("clipCompact",true);buttons[3].getProperties().set("miniLed",true);buttons[6].getProperties().set("reverse",true);buttons[0].getProperties().set("reverse",true);buttons[3].getProperties().set("reverse",true);knobs[0].setTooltip("Ganancia de entrada antes de las dos etapas");knobs[1].setTooltip("Armonicos pares");knobs[2].setTooltip("Armonicos impares");knobs[5].setTooltip("Cantidad de inflado: 0 a 100 por ciento");knobs[6].setTooltip("Curva de inflado: -50 a +50");knobs[7].setTooltip("Ganancia final despues de ambas etapas; sin auto gain");knobs[8].setTooltip("Mid gain before tone filters");knobs[9].setTooltip("Side gain before tone filters");startTimerHz(30);
}
D2FFaceplate::~D2FFaceplate(){stopTimer();setLookAndFeel(nullptr);}
void D2FFaceplate::resized(){
 controlsLayer.setBounds(0,0,1680,252);controlsLayer.setTransform(juce::AffineTransform::scale(.8f).translated(191.7f,21.7f));
 for(int i=0;i<10;i++)knobs[(size_t)i].setBounds(knobCentres[i]-42,81,84,109);
 knobs[1].setBounds(300,65,103,56);knobs[2].setBounds(300,147,103,56);
 knobs[8].setBounds(1332,65,103,56);knobs[9].setBounds(1332,147,103,56);knobs[5].setBounds(1212,65,103,56);knobs[6].setBounds(1212,147,103,56);
 for(int i=0;i<2;i++){int cx=knobCentres[i+3];frequencies[(size_t)i].setBounds(cx-50,76,100,100);knobs[(size_t)(i+3)].setBounds(cx-42,84,84,154);}
 buttons[0].setBounds(84,137,50,36);buttons[2].setBounds(408,58,60,32);buttons[1].setBounds(408,119,60,32);buttons[7].setBounds(408,180,60,32);buttons[6].setBounds(808,58,60,32);buttons[4].setBounds(808,119,60,32);buttons[5].setBounds(808,180,60,32);buttons[3].setBounds(1568,112,76,24);
}
void D2FFaceplate::paint(juce::Graphics& g){
 g.fillAll(colour(0xff090a0d));juce::Rectangle<float> plate(5,5,1624,242);
 auto metal=juce::ColourGradient(colour(0xffe4e6e9),0,5,colour(0xffa2a8ae),0,252,false);metal.addColour(.45,colour(0xffdce0e3));metal.addColour(.94,colour(0xffc7cbd0));g.setGradientFill(metal);g.fillRoundedRectangle(plate,7);
 for(int y=8;y<245;y++){g.setColour((y%4<2?juce::Colours::white:juce::Colours::black).withAlpha(y%4==0?.18f:.035f));g.drawHorizontalLine(y,8,1626);}
 g.setColour(juce::Colours::white.withAlpha(.6f));g.drawRoundedRectangle(plate.reduced(1),6,1);
 for(float x:{19.f,1584.f})for(float y:{28.f,201.f}){g.setColour(colour(0xff646970));g.fillRoundedRectangle(x,y,31,23,11);g.setColour(colour(0xff080a0d));g.fillRoundedRectangle(x+2,y+2,27,19,9);}
 for(float x:{167.f,1558.f})for(float y:{28.f,221.f})screw(g,x,y);
 juce::Graphics::ScopedSaveState contentState(g);g.addTransform(juce::AffineTransform::scale(.8f).translated(191.7f,21.7f));
 g.setColour(colour(0xff292330));text(g,"D2F",61,193,98,42,32);text(g,"POWER",65,102,92,18,14);text(g,"ON",53,145,30,17,10);text(g,"OFF",137,145,30,17,10);
 lamp(g,{80,32,60,60},p.state.getRawParameterValue("power")->load()>.5f);
 const char* names[]{"INPUT GAIN","EVEN","ODD","AIR / HIGH","SUB / LOW","INFLATE","CURVE","OUTPUT GAIN"};
 for(int i=0;i<8;i++){if(i==1||i==2||i==5||i==6)continue;float cx=(float)knobCentres[i];g.setColour(colour(0xff24212b));text(g,names[i],cx-50,51,100,20,12);for(int k=0;k<=10;k++){if(i==3||i==4)continue;float angle=juce::MathConstants<float>::pi*(7.f/6+5.f/3*k/10);float dx=std::sin(angle),dy=-std::cos(angle);g.fillEllipse(cx+dx*43-1,124.5f+dy*43-1,2,2);}}
 for(int group=0;group<3;group++)for(int i=0;i<2;i++){float cx=group==0?328.f:group==1?1360.f:1240.f,cy=i==0?93.f:175.f;g.setColour(colour(0xff24212b));const char* title=group==0?(i==0?"EVEN HARM.":"ODD HARM."):group==1?(i==0?"MID / CENTER":"SIDE / STEREO"):(i==0?"INFLATE":"CURVE");text(g,title,cx-50,i==0?43:125,100,18,11);for(int k=0;k<=10;k++){float angle=juce::MathConstants<float>::pi*(7.f/6+5.f/3*k/10);g.fillEllipse(cx+std::sin(angle)*31-1,cy-std::cos(angle)*31-1,2,2);}}
 const char* af[]{"20","30","40","50"};const char* sf[]{"40","50","60","100"};
 for(int n=0;n<2;n++){float cx=(float)knobCentres[n+3];int selected=(int)p.state.getRawParameterValue(n==0?"airFreq":"subFreq")->load();for(int k=0;k<4;k++){float angle=juce::MathConstants<float>::pi*(7.f/6+5.f/3*k/3);float x=cx+std::sin(angle)*65,y=126-std::cos(angle)*65;g.setColour(colour(k==selected?0xff7305ae:0xff59515f));text(g,n==0?af[k]:sf[k],x-16,y-7,32,14,10,k==selected);}g.setColour(colour(0xff292330));text(g,n==0?"kHz":"Hz",cx-24,194,48,13,9,false);}
 g.setColour(colour(0xff24212b));text(g,"CLIP",1550,92,100,18,11);
 g.setColour(colour(0xff59515f));text(g,"METERING",388,18,100,17,10,false);text(g,"HARMONICS",278,18,100,17,10,false);text(g,"TONE",920,18,220,17,10,false);text(g,"MID / SIDE",1310,18,100,17,10,false);text(g,"INFLATE",1130,18,220,17,10,false);
 g.setColour(colour(0xff24212b));text(g,"Digital To File",480,43,308,22,16);
 constexpr float kValues[]{-21,-18,-16,-14,-12,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,-.5f,0,.5f,1,2,3,4,6};
 const char* kLabels[]{"-21","-18","-16","-14","-12","-10","-9","-8","-7","-6","-5","-4","-3","-2","-1","-.5","0",".5","1","2","3","4","CLIP"};
 constexpr float dValues[]{-60,-54,-48,-42,-36,-30,-27,-24,-21,-18,-15,-12,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0};
 const char* dLabels[]{"-60","-54","-48","-42","-36","-30","-27","-24","-21","-18","-15","-12","-10","-9","-8","-7","-6","-5","-4","-3","-2","-1","CLIP"};
 const bool k12=p.state.getRawParameterValue("inflateOn")->load()>.5f;const bool powered=p.state.getRawParameterValue("power")->load()>.5f;const bool meterOut=p.state.getRawParameterValue("fobosOn")->load()>.5f;
 for(int meter=0;meter<2;meter++){
  const float top=meter==0?80.f:144.f;const float* values=meter==0?dValues:kValues;const char* const* labels=meter==0?dLabels:kLabels;
  g.setColour(colour(0xff292330));text(g,meter==0?"dBFS PEAK":(k12?"K-12 RMS":"K-14 RMS"),480,top-13,308,11,9,false);
  const juce::Rectangle<float> bezel(480,top,308,50);g.setColour(juce::Colours::white.withAlpha(.72f));g.fillRoundedRectangle(bezel.expanded(1),3);g.setColour(colour(0xff08090a));g.fillRoundedRectangle(bezel,2);g.setColour(colour(0xff73777b));g.drawRoundedRectangle(bezel.reduced(1),2,1);
  for(int ch=0;ch<2;ch++){
   const float peak=meterOut?p.outPeakLR[ch].load():p.inPeakLR[ch].load();const float rms=(meterOut?p.outK12LR[ch].load():p.inK12LR[ch].load())+(k12?0.f:2.f);const float level=meter==0?peak:rms;const float y=top+(ch==0?14.f:29.f);
   g.setColour(colour(0xffdfdfe1));text(g,ch==0?"L":"R",483,y-1,10,11,9);
   for(int i=0;i<23;i++){float x=497+i*12.5f;bool on=powered&&(i==22?peak>=0:level>=values[i]);
    if(on){g.setGradientFill(juce::ColourGradient(colour(0xffff2929).withAlpha(.45f),x+4,y+4,juce::Colours::transparentBlack,x+13,y+4,true));g.fillEllipse(x-4,y-4,18,18);}
    g.setColour(colour(0xff020303));g.fillRect(x-1,y-1,11.f,10.f);g.setColour(colour(on?0xffff3030:0xff41191c));g.fillRect(x,y,9.f,8.f);if(on){g.setColour(colour(0xffff8173));g.fillRect(x+1,y+1,7.f,2.f);}
    if(ch==0){g.setColour(colour(0xffdfdfe1));if(i<=16)text(g,labels[i],x-3,top+2,15,9,7.f);text(g,labels[i],x-3,top+40,i==22?20.f:15.f,9,i==22?6.f:7.f);}
   }
  }
 }
 g.setColour(colour(0xff292330));
 const char* leftTitles[]{"K-12 / K-14","METER IN / OUT","DELTA"};const char* rightTitles[]{"MONO / STEREO","BAND SPLIT","PROCESS ORDER"};
 for(int i=0;i<3;i++){text(g,leftTitles[i],388,39.f+i*61,100,18,9);text(g,rightTitles[i],790,39.f+i*61,100,18,9);}
 g.setColour(colour(0xff59515f));text(g,"ON / OFF",393,222,100,14,9,false);text(g,"A / B",790,222,100,14,10,false);text(g,"ON / OFF",790,151,100,10,8,false);
    // Hand-drawn vector script, independent of installed fonts.
    juce::Path signature;
    signature.startNewSubPath(0,22);signature.lineTo(12,2);signature.cubicTo(25,-3,25,7,13,12);signature.lineTo(7,12);
    signature.startNewSubPath(14,18);signature.cubicTo(21,6,29,9,22,18);signature.cubicTo(18,24,13,22,18,16);signature.lineTo(28,12);signature.lineTo(24,20);signature.lineTo(34,14);
    signature.startNewSubPath(29,29);signature.lineTo(39,10);signature.cubicTo(49,5,48,15,40,19);signature.lineTo(36,18);signature.lineTo(49,13);
    signature.startNewSubPath(51,10);signature.lineTo(46,20);signature.quadraticTo(49,23,57,14);
    signature.startNewSubPath(54,6);signature.lineTo(55,4);
    signature.startNewSubPath(54,17);signature.cubicTo(66,16,67,9,61,11);signature.cubicTo(50,17,56,26,68,17);
    signature.cubicTo(82,1,80,-3,73,8);signature.lineTo(68,19);signature.quadraticTo(68,23,78,16);
    signature.cubicTo(86,6,93,10,86,18);signature.cubicTo(79,26,77,16,84,13);signature.lineTo(91,11);signature.lineTo(81,29);signature.cubicTo(75,37,70,29,88,22);
    signature.lineTo(95,13);signature.lineTo(91,20);signature.quadraticTo(92,24,101,14);signature.lineTo(98,20);signature.quadraticTo(101,22,108,14);
    signature.cubicTo(116,7,122,10,115,18);signature.cubicTo(108,26,106,17,112,13);signature.lineTo(120,11);signature.lineTo(116,20);signature.lineTo(127,13);
    signature.startNewSubPath(121,29);signature.lineTo(132,10);signature.cubicTo(142,5,142,15,134,19);signature.lineTo(129,18);signature.lineTo(143,13);
    signature.cubicTo(153,6,156,12,148,19);signature.cubicTo(140,25,137,17,146,12);signature.quadraticTo(149,17,155,13);
    const auto transform=juce::AffineTransform::scale(1.0f,1.0f).translated(556,204);
    g.setColour(juce::Colours::white.withAlpha(0.28f));g.strokePath(signature,juce::PathStrokeType(3.0f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded),transform.followedBy(juce::AffineTransform::translation(1,1)));
    g.setColour(colour(0xff111719));g.strokePath(signature,juce::PathStrokeType(3.0f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded),transform);
    juce::Path flourish;flourish.startNewSubPath(559,235);flourish.lineTo(713,227);flourish.lineTo(662,239);flourish.lineTo(675,230);g.strokePath(flourish,juce::PathStrokeType(1.6f));
}

Phobos67Editor::Phobos67Editor(Phobos67Processor& p):AudioProcessorEditor(p),faceplate(p){
 addAndMakeVisible(faceplate);setResizable(true,true);setResizeLimits(817,126,3268,504);getConstrainer()->setFixedAspectRatio(1634.0/252.0);setSize(817,126);
}
void Phobos67Editor::resized(){faceplate.setBounds(0,0,1634,252);faceplate.setTransform(juce::AffineTransform::scale(juce::jmin(getWidth()/1634.f,getHeight()/252.f)));}
