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
#include "cinder/Signals.h"
#include "cinder/ip/Resize.h"



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
        class TextEdit;
        class Slider;
        class PathEditor;
        class Separator;
        class XYGraph;
    }
    
    namespace geom
    {
        class SuperShape2D;
    }
}

using namespace emergent::ui;

class emergent::ui::UiComponentBase
{
public:
    UiComponentBase() : m_BackgroundColor(ColorA(1,1,1,0)), m_ForegroundColor(ColorA(1,1,1,1)) {};
    UiComponentBase(Rectf r) : m_Rect(r), m_BackgroundColor(ColorA(1,1,1,0)), m_ForegroundColor(ColorA(1,1,1,1)){};
    UiComponentBase(Rectf r, ColorA bg, ColorA fg) : m_Rect(r), m_BackgroundColor(bg), m_ForegroundColor(fg) {};
    
    Rectf   getBoundingBox(){return m_Rect;}
    ColorA  getBackgroundColor(){return m_BackgroundColor;}
    ColorA  getForegroundColor(){return m_ForegroundColor;}
    vec2    getCenter(){return m_Rect.getCenter();}
    
    string  m_Name;
    void    setName(string name) { m_Name = name; }
    
    void setBackgroundColor(ColorA c) {m_BackgroundColor = c;}
    void setForegroundColor(ColorA c) {m_ForegroundColor = c;}
    
protected:
    Rectf   m_Rect;
    ColorA  m_BackgroundColor;
    ColorA  m_ForegroundColor;
    bool    m_RetainMouseControl = false;
    
    bool inBounds(MouseEvent &event)
    {
        return m_Rect.contains(event.getPos()) ? true : false;
    }
};

class emergent::ui::Separator : public UiComponentBase
{
public:
    Separator(vec2 p1, vec2 p2) : m_P1(p1), m_P2(p2){}
    Separator(Rectf r) : UiComponentBase(r){}
    
    void draw(cairo::Context &c)
    {
        c.setSource(m_ForegroundColor);
        c.line(m_P1, m_P2);
        c.stroke();
    }
    
    vec2 m_P1, m_P2;
};

class emergent::ui::PathEditor : public UiComponentBase
{
public:
    PathEditor(Rectf r) : UiComponentBase(r)
    {
        getWindow()->getSignalMouseUp().connect(0,std::bind(&PathEditor::mouseUp, this, std::placeholders::_1));
        getWindow()->getSignalMouseDown().connect(0,std::bind(&PathEditor::mouseDown, this, std::placeholders::_1));
        getWindow()->getSignalMouseDrag().connect(0,std::bind(&PathEditor::mouseDrag, this, std::placeholders::_1));
        
        console() << "path points " << m_Path.getPoints().size() << "\n";
        
        vec2 ul = m_Rect.getUpperLeft();
        
        m_Path.moveTo(vec2(0,0) + ul);
        m_Path.curveTo(vec2(0,0) + ul, m_Rect.getSize() + ul, m_Rect.getSize() + ul);
        
        initHandles();
    }
    
    void initHandles()
    {
        m_Handles.clear();
        
        for(auto it = std::begin(m_Path.getPoints()); it != std::end(m_Path.getPoints()); it++)
        {
            double x  = (*it).x;
            double y  = (*it).y;
            
            Handle h;
            h.bounds = Rectf(x - m_HandleRadius/2.0, y - m_HandleRadius/2.0, x + m_HandleRadius/2.0, y + m_HandleRadius/2.0);
            h.pos = (*it);
             
            m_Handles.push_back(h);
        }
    }
    
    void mouseDown(MouseEvent &event)
    {
        if(inBounds(event))
        {
            m_RetainMouseControl = true;
            event.setHandled(true);
            
            for(auto it = std::begin(m_Handles); it != std::end(m_Handles); it++)
            {
                if((*it).inBounds(event))
                {
                    (*it).active = true;
                }
                else
                {
                    (*it).active = false;
                }
            }
        }
        else
        {
            m_RetainMouseControl = false;
        }
    }
    
    void mouseUp(MouseEvent &event)
    {
        if(m_RetainMouseControl)
        {
            m_RetainMouseControl = false;
            event.setHandled(true);
            
            updatePath();
            initHandles();
            
            for(auto it = std::begin(m_Handles); it != std::end(m_Handles); it++){ (*it).active = false;}
        }
    }
    
    void mouseDrag(MouseEvent &event)
    {
        if(m_RetainMouseControl && inBounds(event))
        {
            event.setHandled(true);
            
            for(auto it = std::begin(m_Handles); it != std::end(m_Handles); it++)
            {
                if((*it).active)
                {
                    (*it).pos = event.getPos();
                    
                    updatePath();
                    break;
                }
            }
            
            //initHandles();
        }
    }
    
    void draw(cairo::Context &c)
    {
        c.setSource(ColorA(0.1,0.1,0.1,0.2));
        c.rectangle(m_Rect);
        c.fill();
        
        c.setSource(ColorA(0.8,0.8,0.8,1));
        c.appendPath(m_Path);
        c.stroke();
        
        for(auto it = std::begin(m_Path.getPoints()); it != std::end(m_Path.getPoints()); it++)
        {
            c.setSource(Color(0.2,0.2,0.2));
            c.circle(*it, 3);
            c.fill();
        }
    }
    
    Path2d m_Path;
    
    void updatePath()
    {
        for(int i = 0; i < m_Handles.size(); i++)
        {
            m_Path.setPoint(i, m_Handles[i].pos);
        }
        
        
    }
    
    Path2d getPath()
    {
        Path2d path = m_Path;
        
        for(int i = 0; i < m_Path.getNumPoints(); i++)
        {
            path.setPoint(i, m_Path.getPoints()[i] - m_Rect.getUpperLeft());
        }
        
        return path;
    }
    
    Path2d getPathCentered()
    {
        Path2d path = m_Path;
        
        for(int i = 0; i < m_Path.getNumPoints(); i++)
        {
            path.setPoint(i, m_Path.getPoints()[i] - m_Rect.getUpperLeft() + getWindowCenter());
        }
        
        return path;
    }
    
    struct Handle
    {
        vec2  pos;
        Rectf bounds;
        bool  active = false;
        
        bool inBounds(MouseEvent &event)
        {
            if(bounds.contains(event.getPos()))
            {
                return true;
            }
            
            return false;
        }
    };
    
    vector<Handle> m_Handles;
    float m_HandleRadius = 5;
};

class emergent::ui::XYGraph : public UiComponentBase
{
public:
    XYGraph()
    {
        //init();
    };
    
    XYGraph(Rectf r) : UiComponentBase(r)
    {
        init();
    };
    
    void mouseDown(MouseEvent &event)
    {
        if(inBounds(event))
        {
            m_RetainMouseControl = true;
            m_CurrentPoint = (vec2)event.getPos() - m_Rect.getUpperLeft();
            m_SelectedColor = m_ColorMapImage.getSurface().getPixel(event.getPos() - (ivec2)m_Rect.getUpperLeft());
            event.setHandled(true);
        }
        else
        {
            m_RetainMouseControl = false;
        }
    }
    
    void mouseDrag(MouseEvent &event)
    {
        if(m_RetainMouseControl && inBounds(event))
        {
            event.setHandled(true);
            
            m_CurrentPoint = (vec2)event.getPos() - m_Rect.getUpperLeft();
            m_SelectedColor = m_ColorMapImage.getSurface().getPixel(event.getPos() - (ivec2)m_Rect.getUpperLeft());
        }
    }
    
    void mouseUp(MouseEvent &event)
    {
        if(m_RetainMouseControl)
        {
            m_RetainMouseControl = false;
            event.setHandled(true);
            m_CurrentPoint = (vec2)event.getPos() - m_Rect.getUpperLeft();
            //m_SelectedColor = m_ColorMapImage.getSurface().getPixel(event.getPos() - (ivec2)m_Rect.getUpperLeft());
        }
    }
    
    void draw(cairo::Context &c)
    {
        vec2 ul = m_Rect.getUpperLeft();
        
        //c.setSource(ColorA(0.1,0.1,0.1,0.2));
        c.setSourceSurface(m_ColorMapImage, m_Rect.getUpperLeft().x, m_Rect.getUpperLeft().y);
        c.rectangle(m_Rect);
        c.fill();
        
        c.setSource(Color::white());
        c.setLineWidth(1);
        c.line(vec2(m_CurrentPoint.x + ul.x, m_Rect.getY1()), vec2(m_CurrentPoint.x + ul.x, m_Rect.getY2()));
        c.line(vec2(m_Rect.getX1(), m_CurrentPoint.y + ul.y), vec2(m_Rect.getX2(), m_CurrentPoint.y + ul.y ));
        c.stroke();
        
        c.setSource(Color::white());
        c.rectangle(m_CurrentPoint.x + ul.x - 1, m_CurrentPoint.y + ul.y - 1, 2, 2);
        c.fill();
        
        //console() << "selected color " << m_SelectedColor;
    }
    
    ColorA m_SelectedColor;
    
    void setSourceImage(string path)
    {
        
    }
    
    
private:
    Surface m_ColorMap;
    cairo::SurfaceImage m_ColorMapImage;
    vec2 m_CurrentPoint;
    
    void init()
    {
        getWindow()->getSignalMouseDown().connect(0,std::bind(&XYGraph::mouseDown, this, std::placeholders::_1));
        getWindow()->getSignalMouseDrag().connect(0,std::bind(&XYGraph::mouseDrag, this, std::placeholders::_1));
        getWindow()->getSignalMouseUp().connect(0,std::bind(&XYGraph::mouseUp, this, std::placeholders::_1));


        m_SelectedColor = ColorA(1,1,1,1);
        m_ColorMap = Surface( loadImage( loadResource("images/color_grid.png") ) );
        m_ColorMapImage = cairo::SurfaceImage( ip::resizeCopy( m_ColorMap, m_ColorMap.getBounds(), m_Rect.getSize() ) );
    }
};


class emergent::ui::Label : public UiComponentBase
{
public:
    enum Alignment {LEFT, RIGHT, CENTER};
    Alignment m_Alignment;
    
    Label(string text, vec2 pos, vec2 size) : m_Text(text), m_Size(size)
    {
        m_Rect = Rectf(pos.x, pos.y, pos.x + m_Size.x, pos.y + m_Size.y);
        setBackgroundColor(ColorA(0,0,0,1));
        m_Alignment = Alignment::LEFT;
    };

    string m_Text = "label";
    double m_TextSize = 11;
    vec2   m_Size;
    
    void setText(string t) { m_Text = t; }
    void setTextSize(int s) { m_TextSize = s; }
    void setAlignment(emergent::ui::Label::Alignment a){ m_Alignment = a; }
    
    void draw(cairo::Context &c)
    {
        c.setSource(m_BackgroundColor);
        c.rectangle(m_Rect);
        c.fill();
        
        c.setSource(m_ForegroundColor);
        c.setFontSize(m_TextSize);
        cairo::TextExtents te = c.textExtents(m_Text);
        
        if(m_Alignment == LEFT)
        {
            c.moveTo(m_Rect.getX1(), m_Rect.getCenter().y + te.height()/2.0);
        }
        else if(m_Alignment == CENTER)
        {
            c.moveTo(m_Rect.getCenter().x - te.width()/2.0, m_Rect.getCenter().y + te.height()/2.0);
        }
        else if(m_Alignment == RIGHT)
        {
            c.moveTo(m_Rect.getX2() - te.width(), m_Rect.getCenter().y + te.height()/2.0);
        }
        
        
        c.showText(m_Text);
        c.moveTo(0,0); // Move back to origin
    }
};

class emergent::ui::TextEdit : public Label
{
public:
    TextEdit(string text, vec2 pos, vec2 size) : Label(text, pos, size)
    {
        getWindow()->getSignalKeyDown().connect(0,std::bind(&TextEdit::keyDown, this, std::placeholders::_1));
        getWindow()->getSignalMouseDown().connect(0,std::bind(&TextEdit::mouseDown, this, std::placeholders::_1));
    }
    
    void mouseDown(MouseEvent &event)
    {
        if(inBounds(event))
        {
            m_RetainKeyboardControl = true;
            m_Text.clear();
            event.setHandled(true);
        }
        else
        {
            m_RetainKeyboardControl = false;
        }
    }
    
    void keyDown(KeyEvent &event)
    {
        if(m_RetainKeyboardControl)
        {
            if(event.getCode() == KeyEvent::KEY_RETURN)
            {
                m_RetainKeyboardControl = false;
                signal_TextChanged.emit(m_Text);
            }
            else if(event.getChar())
            {
                m_Text = m_Text + event.getChar();
            }
            
            event.setHandled(true);
        }
    }
    
    typedef ci::signals::Signal< void(string) > SignalText;
    SignalText signal_TextChanged;
    
private:
    bool m_RetainKeyboardControl = false;
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
            
            if(m_Mode == MOMENTARY)
            {
                setState(ON);
            }
            else if(m_Mode == TOGGLE)
            {
                setState(!m_State);
                signal_ValueChanged.emit(m_State);
            }
            
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
        if(m_RetainMouseControl)
        {
            m_RetainMouseControl = false;
            
            if(m_Mode == MOMENTARY)
            {
                setState(OFF);
                signal_Clicked.emit();
            }
            
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
    
    string m_OnText = "O N";
    string m_OffText = "O F F";
    
    ColorA m_BackgroundColorOn  = ColorA(0.5,0.5,0.5,1.0);
    ColorA m_ForegroundColorOn  = ColorA(0.8,0.8,0.8,1.0);
    
    ColorA m_BackgroundColorOff = ColorA(0.8,0.8,0.8,1.0);
    ColorA m_ForegroundColorOff = ColorA(0.5,0.5,0.5,1.0);
    
    void setBackgroundOff(ColorA c) { m_BackgroundColorOff = c; setState(m_State); }
    void setBackgroundOn(ColorA c)  { m_BackgroundColorOn = c;  setState(m_State); }
    void setForegroundOff(ColorA c) { m_ForegroundColorOff = c; setState(m_State); }
    void setForegroundOn(ColorA c)  { m_ForegroundColorOn = c;  setState(m_State); }
    void setOnOffText(string onText, string offText){m_OnText = onText; m_OffText = offText;}
    
    typedef ci::signals::Signal< void() > SignalClicked;
    SignalClicked signal_Clicked;
    
    typedef ci::signals::Signal<void(int)> SignalValueChanged;
    SignalValueChanged signal_ValueChanged;
    
private:
    void setState(int s)
    {
        m_State = s;
        
        if(m_State == ON)
        {
            m_ForegroundColor = m_ForegroundColorOn;
            m_BackgroundColor = m_BackgroundColorOn;
            
            m_Text = m_Mode == TOGGLE ? m_OnText : m_Text;
        }
        else if(m_State == OFF)
        {
            m_ForegroundColor = m_ForegroundColorOff;
            m_BackgroundColor = m_BackgroundColorOff;
            
            m_Text = m_Mode == TOGGLE ? m_OffText : m_Text;
        }
    }
};

class emergent::ui::Slider : public UiComponentBase
{
    
public:
    Slider(Rectf r, ColorA bg = ColorA(0.1,0.1,0.1,1), ColorA fg = ColorA(0.9,0.9,0.9,9)) : UiComponentBase(r, bg, fg)
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
            signal_SliderChanged.emit(this);
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
            signal_SliderChanged.emit(this);
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
    typedef ci::signals::Signal<void(Slider*)> SignalSliderChanged;
    SignalSliderChanged signal_SliderChanged;

};

class emergent::ui::UiView
{
    
public:
    UiView() : m_Rect(0,0,170,getWindowHeight()), m_HeaderRect(0,0,120,100)
    {
        m_Font = Font(loadResource("fonts/InputSans-ExtraLight.ttf"), 10);
    }
    
    Rectf m_Rect;
    Rectf m_HeaderRect;
    
    void update()
    {
        m_Rect = Rectf(0,0,170,getWindowHeight());
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
        
        // --------------- TextEdits
        for(auto it = std::begin(m_TextEdits); it != std::end(m_TextEdits); it++)
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
        
        // --------------- Color Selectors
        for(auto it = std::begin(m_ColorSelectors); it != std::end(m_ColorSelectors); it++)
        {
            (*it)->draw(ctx);
        }
        
        // --------------- Path Editors
        for(auto it = std::begin(m_PathEditors); it != std::end(m_PathEditors); it++)
        {
            (*it)->draw(ctx);
        }
        
        // --------------- Separators
        for(auto it = std::begin(m_Separators); it != std::end(m_Separators); it++)
        {
            (*it)->draw(ctx);
        }
        
        // --------------- XY Graphs
        for(auto it = std::begin(m_XYGraphs); it != std::end(m_XYGraphs); it++)
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
    
    void addTextEdit(TextEdit *t)
    {
        m_TextEdits.push_back(t);
    }
    
    void addColorSelector(ColorSelector *cs)
    {
        m_ColorSelectors.push_back(cs);
    }
    
    void addPathEditor(PathEditor *pe)
    {
        m_PathEditors.push_back(pe);
    }
    
    void addSeparator(Separator *s)
    {
        m_Separators.push_back(s);
    }
    
    void addXYGraph(XYGraph *xyg)
    {
        m_XYGraphs.push_back(xyg);
    }
    
private:
    cairo::SurfaceImage     m_BgSurface;
    vector<Slider *>        m_Sliders;
    vector<Label  *>        m_Labels;
    vector<Button *>        m_Buttons;
    vector<TextEdit *>      m_TextEdits;
    vector<ColorSelector *> m_ColorSelectors;
    vector<PathEditor *>    m_PathEditors;
    vector<Separator *>     m_Separators;
    vector<XYGraph *>       m_XYGraphs;
    
    Font    m_Font;
};

#endif /* eio_ui_h */
