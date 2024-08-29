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

    sf::Vector2i pos;

    sf::Event event;
    while ( WindowRunning )
    {
        while ( ImageViewer::window.pollEvent( event ) )
        {
            switch ( event.type )
            {
                case sf::Event::Closed:
                    WindowRunning = FALSE;
                    Hotkey::terminate();
                    break;
                
                case sf::Event::MouseWheelScrolled:
                    ImageViewer::zoomImage( event );
                    break;

                case sf::Event::MouseButtonPressed:
                    ImageViewer::draggableImage = TRUE;
                    ImageViewer::mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition( ImageViewer::window ));
                    break;
                
                case sf::Event::MouseMoved:
                    if ( ImageViewer::draggableImage )
                        ImageViewer::dragImage( event );
                    break;

                case sf::Event::MouseButtonReleased:
                    ImageViewer::draggableImage = FALSE;
                    break;

                default:
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
    Hotkey::add_hotkey( {VK_OEM_COMMA, 0x31},       Hotkey::terminate, NULL, FALSE );
    Hotkey::add_hotkey( {VK_UP},              ImageViewer::showWindow, NULL,  TRUE );
    Hotkey::add_hotkey( {VK_DOWN},            ImageViewer::hideWindow, NULL,  TRUE );
    Hotkey::add_hotkey( {VK_LEFT},              ImageViewer::nextPage, NULL,  TRUE );
    Hotkey::add_hotkey( {VK_RIGHT},             ImageViewer::prevPage, NULL,  TRUE );
}

void Hotkey::terminate()
{
    WindowRunning = FALSE;
    PostThreadMessage( threadID, WM_EXIT, 0, 0 );
}



