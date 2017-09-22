#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "eio_ui.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace emergent::ui;

void prepareSettings(App::Settings *settings)
{
    settings->setTitle("Cinder UI Examples");
    settings->setFullScreen(false);
}

class cinder_ui_exampleApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    UiView           m_UiView;
    Slider          *m_AtomSize;
    Slider          *m_NumRings;
    Button          *m_ScaleAtoms;
    Slider          *m_NumAtoms;
    Slider          *m_ScaleRings;
    XYGraph         *m_AtomColorSelector;
    XYGraph         *m_BackgroundColorSelector;
    Label           *m_CirclesLabel;
    Separator       *m_CirclesSeparator;
    
    
    Label           *m_PathsLabel;
    Separator       *m_PathsSeparator;
    PathEditor      *m_PathEditor;
    Slider          *m_PathRotations;
    XYGraph         *m_XYGraph;
    ButtonMatrix    *m_ButtonMatrix;
};

void cinder_ui_exampleApp::setup()

{
    float width = 150;
    float height = 20;
    float yMargin = 10;
    
    // ---------------------------------------- CIRCLES
    vec2 p = vec2(10,10);
    m_CirclesLabel = new Label("L A B E L 1", p, vec2(width, height));
    m_CirclesLabel->setBackgroundColor(ColorA(0,0,0,0));
    m_CirclesLabel->setTextSize(13);
    m_UiView.addLabel(m_CirclesLabel);
    
    p = vec2(p.x, p.y + height);
    m_CirclesSeparator = new Separator(p, p + vec2(width, 0));
    m_UiView.addSeparator(m_CirclesSeparator);
    
    p = vec2(p.x, p.y + yMargin);
    m_AtomSize = new Slider(Rectf(p.x,p.y,p.x + width, p.y + height));
    m_UiView.addSlider(m_AtomSize);
    
    p = vec2(p.x, p.y + height + yMargin);
    m_NumRings = new Slider(Rectf(p.x,p.y,p.x + width, p.y + height));
    m_UiView.addSlider(m_NumRings);
    
    p = vec2(p.x, p.y + height + yMargin);
    m_NumAtoms = new Slider(Rectf(p.x,p.y,p.x + width, p.y + height));
    m_UiView.addSlider(m_NumAtoms);
    
    p = vec2(p.x, p.y + height + yMargin);
    m_ScaleAtoms = new Button("T O G G L E", p, vec2(width, height), Button::TOGGLE);
    m_ScaleAtoms->setOnOffText("O N", "O F F");
    m_UiView.addButton(m_ScaleAtoms);
    
    p = vec2(p.x, p.y + height + yMargin);
    m_ScaleRings = new Slider(Rectf(p.x,p.y,p.x + width, p.y + height));
    m_UiView.addSlider(m_ScaleRings);
    
    p = vec2(p.x, p.y + height + yMargin);
    m_AtomColorSelector = new XYGraph(Rectf(p.x,p.y,p.x + width, p.y + height*4.0));
    m_UiView.addXYGraph(m_AtomColorSelector);
    
    p = vec2(p.x, p.y + height*4.0 + yMargin);
    m_BackgroundColorSelector = new XYGraph(Rectf(p.x,p.y,p.x + width, p.y + height*4.0));
    m_UiView.addXYGraph(m_BackgroundColorSelector);
    
    
    // ---------------------------------------- PATHS
    p = vec2(p.x, p.y + height*4.0 + yMargin);
    m_PathsLabel = new Label("L A B E L 2", p, vec2(width, height));
    m_PathsLabel->setBackgroundColor(ColorA(0,0,0,0));
    m_PathsLabel->setTextSize(13);
    m_UiView.addLabel(m_PathsLabel);
    
    p = vec2(p.x, p.y + height);
    m_PathsSeparator = new Separator(p, p + vec2(width, 0));
    m_UiView.addSeparator(m_PathsSeparator);
    
    p = vec2(p.x, p.y + yMargin);
    m_PathEditor = new PathEditor(Rectf(p.x,p.y,p.x + width, p.y + height*7.0));
    m_UiView.addPathEditor(m_PathEditor);
    
    p = vec2(p.x, p.y + height*7.0 + yMargin);
    m_PathRotations = new Slider(Rectf(p.x,p.y,p.x + width, p.y + height));
    m_UiView.addSlider(m_PathRotations);
    
    p = vec2(p.x, p.y + height + yMargin);
    m_ButtonMatrix = new ButtonMatrix(0, p, vec2(20,20), vec2(100,100));
    m_UiView.addButtonMatrix(m_ButtonMatrix);
}

void cinder_ui_exampleApp::mouseDown( MouseEvent event )
{
}

void cinder_ui_exampleApp::update()
{
    m_UiView.update();
    m_UiView.setRect(Rectf(0,0,400,getWindowHeight()));
    
}

void cinder_ui_exampleApp::draw()
{
	gl::clear( Color( 0.5, 0.5, 0.5 ) );
    
    m_UiView.draw();
}

CINDER_APP( cinder_ui_exampleApp, RendererGl )
