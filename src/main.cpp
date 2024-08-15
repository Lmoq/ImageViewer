#include <iostream>
#include <image.h>
#include <main.h>
#include <hotkey.h>

static BOOL WindowRunning = TRUE;
static sf::Clock winClock;

static UINT8 FPS = 60;
static UINT8 FrameTime = 1000 / FPS;

static sf::Int32 timeleft;
static sf::Int32 lastframetime = winClock.getElapsedTime().asMilliseconds();

int main()
{
    ImageViewer::open( MIHON );

    INIT_HOTKEY();
    Hotkey::run();

    sf::Event event;
    while ( WindowRunning )
    {
        while ( ImageViewer::window.pollEvent( event ) )
        {
            switch ( event.type )
            {
                case sf::Event::Closed:
                    WindowRunning = TRUE;
                    PostQuitMessage(0);
                    break;
                
                case sf::Event::MouseWheelScrolled:
                    ImageViewer::zoomImage( event );
                    break;
            }
        }

        if ( ImageViewer::displayWindow )
        {
            ImageViewer::window.clear( ImageViewer::ClearColor );
            ImageViewer::window.draw( ImageViewer::sprite );
            ImageViewer::window.display();
        }

        timeleft = FrameTime - ( winClock.getElapsedTime().asMilliseconds() - lastframetime );
        if ( timeleft > 0 && timeleft <= FrameTime )
        {
            sf::sleep( sf::milliseconds( timeleft ) );
        }
        lastframetime = winClock.getElapsedTime().asMilliseconds();
    }
    ImageViewer::window.close();
    Hotkey::wait();
}

void INIT_HOTKEY()
{
    Hotkey::add_hotkey( {VK_OEM_COMMA, 0x31},              terminate_, FALSE );
    Hotkey::add_hotkey( {VK_UP},              ImageViewer::showWindow,  TRUE );
    Hotkey::add_hotkey( {VK_DOWN},            ImageViewer::hideWindow,  TRUE );
    Hotkey::add_hotkey( {VK_LEFT},              ImageViewer::nextPage,  TRUE );
    Hotkey::add_hotkey( {VK_RIGHT},             ImageViewer::prevPage,  TRUE );
}

void terminate_()
{
    PostQuitMessage(0);
    WindowRunning = FALSE;
}



