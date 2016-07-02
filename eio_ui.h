//
//  eio_ui.h
//  CarioDebugView
//
//  Created by connerlacy on 7/1/16.
//
//

#ifndef eio_ui_h
#define eio_ui_h

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/cairo/Cairo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace emergent
{
    namespace ui
    {
        class UiView;
        class UiComponentBase;
        class Label;
        class Button;
        class Slider;
    }
}

using namespace emergent::ui;

class emergent::ui::UiComponentBase
{
public:
    UiComponentBase() : m_BackgroundColor(ColorA(1,1,1,0)), m_ForegroundColor(ColorA(1,1,1,1)) {};
    UiComponentBase(Rectf r, ColorA bg, ColorA fg) : m_Rect(r), m_BackgroundColor(bg), m_ForegroundColor(fg) {}
    
    Rectf   getBoundingBox(){return m_Rect;}
    ColorA  getBackgroundColor(){return m_BackgroundColor;}
    ColorA  getForegroundColor(){return m_ForegroundColor;}
    
protected:
    Rectf   m_Rect;
    ColorA  m_BackgroundColor;
    ColorA  m_ForegroundColor;
    bool    m_RetainMouseControl = false;
    
    bool inBounds(MouseEvent &event)
    {
        return m_Rect.contains(event.getPos()) ? true : false;
    }
    
    void setBackgroundColor(ColorA c) {m_BackgroundColor = c;}
    void setForegroundColor(ColorA c) {m_ForegroundColor = c;}
    
};

class emergent::ui::Label : public UiComponentBase
{

public:
    
    Label(string text, vec2 pos, vec2 size) : m_Text(text), m_Size(size)
    {
        m_Rect = Rectf(pos.x, pos.y, pos.x + m_Size.x, pos.y + m_Size.y);
        setBackgroundColor(ColorA(0,0,0,1));
    };
    
    string m_Text = "label";
    double    m_TextSize = 18;
    vec2   m_Size;
    
    void setText(string t) { m_Text = t; }
    void setTextSize(int s) { m_TextSize = s; }
    
    void draw(cairo::Context &c)
    {
        c.setSource(m_BackgroundColor);
        c.rectangle(m_Rect);
        c.fill();
        
        c.setSource(m_ForegroundColor);
        c.setFontSize(m_TextSize);
        cairo::TextExtents te = c.textExtents(m_Text);
        c.moveTo(m_Rect.getCenter().x - te.width()/2.0, m_Rect.getCenter().y + te.height()/2.0);
        c.showText(m_Text);
        c.moveTo(0,0); // Move back to origin
    }
};

class emergent::ui::Button : public Label
{
    
public:
    Button(string text, vec2 pos, vec2 size, int mode = MOMENTARY) :
    Label(text, pos, size),
    m_Mode(mode)
    {
        m_BackgroundColor = m_BackgroundColorOff;
        m_ForegroundColor = m_ForegroundColorOff;
        getWindow()->getSignalMouseDown().connect(0,std::bind(&Button::mouseDown, this, std::placeholders::_1));
        getWindow()->getSignalMouseDrag().connect(0,std::bind(&Button::mouseDrag, this, std::placeholders::_1));
        getWindow()->getSignalMouseUp().connect(0, std::bind(&Button::mouseUp, this, std::placeholders::_1));
    }
    
    void mouseDown(MouseEvent &event)
    {
        if(inBounds(event))
        {
            m_RetainMouseControl = true;
            m_ForegroundColor = m_ForegroundColorOn;
            m_BackgroundColor = m_BackgroundColorOn;
            event.setHandled(true);
        }
    }
    
    void mouseDrag(MouseEvent &event)
    {
        if(m_RetainMouseControl)
        {
            event.setHandled(true);
        }
    }
    
    void mouseUp(MouseEvent &event)
    {
        if(m_Mode == MOMENTARY)
        {
            m_ForegroundColor = m_ForegroundColorOff;
            m_BackgroundColor = m_BackgroundColorOff;
        }
        
        if(m_RetainMouseControl)
        {
            m_RetainMouseControl = false;
            //signal_Clicked.emit();
            event.setHandled(true);
        }
    }

    void draw(cairo::Context &c)
    {
        c.setSource(m_BackgroundColor);
        c.rectangle(m_Rect);
        c.fill();
        
        c.setSource(m_ForegroundColor);
        c.setFontSize(m_TextSize);
        cairo::TextExtents te = c.textExtents(m_Text);
        c.moveTo(m_Rect.getCenter().x - te.width()/2.0, m_Rect.getCenter().y + te.height()/2.0);
        c.showText(m_Text);
        c.moveTo(0,0); // Move back to origin
    }
    
    enum MODE   {MOMENTARY, TOGGLE};
    enum STATE  {ON,OFF};
    
    int m_State = OFF;
    int m_Mode  = MOMENTARY;
    
    ColorA m_BackgroundColorOn  = ColorA(0.5,0.5,0.5,1.0);
    ColorA m_ForegroundColorOn  = ColorA(0.8,0.8,0.8,1.0);
    
    ColorA m_BackgroundColorOff = ColorA(0.8,0.8,0.8,1.0);
    ColorA m_ForegroundColorOff = ColorA(0.5,0.5,0.5,1.0);
    
    void setBackgroundOff(ColorA c) { m_BackgroundColorOff = c; }
    void setBackgroundOn(ColorA c)  { m_BackgroundColorOn = c;  }
    void setForegroundOff(ColorA c) { m_ForegroundColorOff = c; }
    void setForegroundOn(ColorA c)  { m_ForegroundColorOn = c; }
    
};

class emergent::ui::Slider : public UiComponentBase
{
    
public:
    Slider(Rectf r, ColorA bg, ColorA fg) : UiComponentBase(r, bg, fg)
    {
        getWindow()->getSignalMouseDown().connect(0,std::bind(&Slider::mouseDown, this, std::placeholders::_1));
        getWindow()->getSignalMouseDrag().connect(0,std::bind(&Slider::mouseDrag, this, std::placeholders::_1));
        getWindow()->getSignalMouseUp().connect(0, std::bind(&Slider::mouseUp, this, std::placeholders::_1));
    }
    
    void mouseDown(MouseEvent &event)
    {
        if(inBounds(event))
        {
            event.setHandled(true);
            m_RetainMouseControl = true;
            vec2 pos = event.getPos();
            m_Value = glm::clamp( (pos.x - m_Rect.getX1()) / (m_Rect.getWidth()), 0.0f , 1.0f);
        }
    }
    
    void mouseUp(MouseEvent &event)
    {
        if(m_RetainMouseControl)
        {
            m_RetainMouseControl = false;
            event.setHandled(true);
        }
    }
    
    void mouseDrag(MouseEvent &event)
    {
        if(m_RetainMouseControl)
        {
            event.setHandled(true);
            vec2 pos = event.getPos();
            m_Value = glm::clamp( (pos.x - m_Rect.getX1()) / (m_Rect.getWidth()), 0.0f , 1.0f);
        }
    }
    
    void draw(cairo::Context &c)
    {
        c.setSource(m_BackgroundColor);
        c.rectangle(m_Rect);
        c.fill();
        
        c.setSource(m_ForegroundColor);
        c.rectangle(m_Rect.getX1(), m_Rect.getY1(), m_Rect.getWidth()*m_Value, m_Rect.getHeight());
        c.fill();
    }
    
    double m_Value = 0.5;

};

class emergent::ui::UiView
{
    
public:
    UiView() : m_Rect(0,0,250,getWindowHeight()), m_HeaderRect(0,0,500,100)
    {
        m_Font = Font(loadResource("fonts/InputSans-ExtraLight.ttf"), 10);
    }
    
    Rectf m_Rect;
    Rectf m_HeaderRect;
    
    void update()
    {
        m_Rect = Rectf(0,0,250,getWindowHeight());
    }
    
    void draw()
    {
        gl::color(1,1,1);
        gl::disableDepthRead();
        gl::disableDepthWrite();
        gl::setMatricesWindow(getWindowSize());
        
        m_BgSurface = cairo::SurfaceImage(m_Rect.getWidth(), m_Rect.getHeight(), true);
        cairo::Context ctx(m_BgSurface);
        ctx.setSource(ColorA(1,1,1,0));
        ctx.paint();
        
        ctx.setSource(ColorA(1,1,1,0.25));
        ctx.rectangle(0, 0, m_Rect.getWidth(), m_Rect.getHeight());
        ctx.fill();
        
        // --------------- Labels
        for(auto it = std::begin(m_Labels); it != std::end(m_Labels); it++)
        {
            (*it)->draw(ctx);
        }
        
        // --------------- Sliders
        for(auto it = std::begin(m_Sliders); it != std::end(m_Sliders); it++)
        {
            (*it)->draw(ctx);
        }
        
        // --------------- Buttons
        for(auto it = std::begin(m_Buttons); it != std::end(m_Buttons); it++)
        {
            (*it)->draw(ctx);
        }
        
        gl::draw( gl::Texture::create(m_BgSurface.getSurface()) );
    }
    
    // ==========================================================
    void addSlider(Slider *s)
    {
        m_Sliders.push_back(s);
    }
    
    void addLabel(Label *l)
    {
        m_Labels.push_back(l);
    }
    
    void addButton(Button *b)
    {
        m_Buttons.push_back(b);
    }
    
private:
    cairo::SurfaceImage m_BgSurface;
    vector<Slider *>    m_Sliders;
    vector<Label  *>    m_Labels;
    vector<Button *>    m_Buttons;
    
    Font    m_Font;
};

#endif /* eio_ui_h */
