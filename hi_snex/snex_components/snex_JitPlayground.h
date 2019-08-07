/*  ===========================================================================
*
*   This file is part of HISE.
*   Copyright 2016 Christoph Hart
*
*   HISE is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option any later version.
*
*   HISE is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with HISE.  If not, see <http://www.gnu.org/licenses/>.
*
*   Commercial licences for using HISE in an closed source project are
*   available on request. Please visit the project's website to get more
*   information about commercial licencing:
*
*   http://www.hartinstruments.net/hise/
*
*   HISE is based on the JUCE library,
*   which also must be licenced for commercial applications:
*
*   http://www.juce.com
*
*   ===========================================================================
*/

#pragma once


namespace snex {
namespace jit {
using namespace juce;

struct SnexPathFactory: public hise::PathFactory
{
    String getId() const override { return "Snex"; }
    Path createPath(const String& id) const override;
};
    
struct Graph : public Component
{
	void setBuffer(AudioSampleBuffer& b);

	void paint(Graphics& g) override;

	Path p;
};

/** Quick and dirty assembly syntax highlighter.

Definitely not standard conform (don't know nothing about assembly lol).
*/
class AssemblyTokeniser : public juce::CodeTokeniser
{
	enum Tokens
	{
		Unknown,
		Comment,
		Location,
		Number,
		Label,
		Instruction
	};

	int readNextToken(CodeDocument::Iterator& source) override;

	CodeEditorComponent::ColourScheme getDefaultColourScheme() override;
};


class SnexPlayground : public Component,
	public ComboBox::Listener,
	public Compiler::DebugHandler
{
public:

    struct ParameterList: public Component
    {
        void setNumSliders(int numSliders)
        {
            sliders.clear();
            
            for(int i = 0; i < numSliders; i++)
            {
                auto s = new juce::Slider("Parameter " + String(i+1));
                s->setLookAndFeel(&laf);
                s->setRange(0.0, 1.0, 0.01);
                s->setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
                
                
                s->setColour(HiseColourScheme::ComponentFillTopColourId, Colour(0x66333333));
                s->setColour(HiseColourScheme::ComponentFillBottomColourId, Colour(0xfb111111));
                s->setColour(HiseColourScheme::ComponentOutlineColourId, Colours::white.withAlpha(0.3f));
                s->setColour(HiseColourScheme::ComponentTextColourId, Colours::white);
                
                addAndMakeVisible(s);
                s->setSize(128, 48);
                sliders.add(s);
            }
            
            auto numColumns = jmax(1, getWidth() / 150);
            auto numRows = sliders.size() / numColumns + 1;
            
            setSize(getWidth(), numRows * 60);
            resized();
        }
        
        void resized() override
        {
            auto numColumns = jmax(1, getWidth() / 150);
            auto numRows = sliders.size() / numColumns + 1;
            
            int x = 0;
            int y = 0;
            int i = 0;
            
            for(int row = 0; row < numRows; row++)
            {
                x = 0;
                
                for(int column = 0; column < numColumns; column++)
                {
                    if(auto s = sliders[i])
                    {
                        sliders[i++]->setTopLeftPosition(x, y + 5);
                        x += 150;
                    }
                    else
                        break;
                }
                
                y += 50;
            }
        }
        
        hise::GlobalHiseLookAndFeel laf;
        juce::OwnedArray<juce::Slider> sliders;
    };
    
    static String getDefaultCode();
    
	SnexPlayground(Value externalCodeValue);

	~SnexPlayground();

	void paint(Graphics& g) override;
	void resized() override;

	void comboBoxChanged(ComboBox* ) override
	{
		recalculate();
	}


	bool keyPressed(const KeyPress& k) override;
	void createTestSignal();

private:

	struct ButtonLaf : public LookAndFeel_V3
	{
		void drawButtonBackground(Graphics& g, Button& b, const Colour& , bool over, bool down)
		{
			float alpha = 0.0f;

			if (over)
				alpha += 0.2f;
			if (down)
				alpha += 0.2f;

			if (b.getToggleState())
			{
				g.setColour(Colours::white.withAlpha(0.5f));
				g.fillRoundedRectangle(b.getLocalBounds().toFloat(), 3.0f);
			}

			g.setColour(Colours::white.withAlpha(alpha));
			g.fillRoundedRectangle(b.getLocalBounds().toFloat(), 3.0f);
		}

		void drawButtonText(Graphics&g, TextButton& b, bool , bool )
		{
			auto c = !b.getToggleState() ? Colours::white : Colours::black;
			g.setColour(c.withAlpha(0.8f));
			g.setFont(GLOBAL_BOLD_FONT());
			g.drawText(b.getButtonText(), b.getLocalBounds().toFloat(), Justification::centred);
		}
	} blaf;

	void logMessage(const String& m) override
	{
		consoleContent.insertText(consoleContent.getNumCharacters(), m);
		consoleContent.clearUndoHistory();
	}

	void recalculate();

    hise::PopupLookAndFeel laf;

	Value externalCodeValue;

	CodeDocument doc;

	AudioSampleBuffer b;
	Graph graph;
	ComboBox testSignal;
	juce::CPlusPlusCodeTokeniser tokeniser;
	
	CodeEditorComponent editor;
	AssemblyTokeniser assemblyTokeniser;
	CodeDocument assemblyDoc;
	CodeEditorComponent assembly;

	Label resultLabel;

	Compiler::Tokeniser consoleTokeniser;
	CodeDocument consoleContent;
	CodeEditorComponent console;
    
    SnexPathFactory factory;
    Path snexIcon;

	TextButton showAssembly;
	TextButton showSignal;
	TextButton showConsole;
	TextButton compileButton;
    
    ParameterList sliders;

};

}
}