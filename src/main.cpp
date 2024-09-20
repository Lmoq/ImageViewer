#include <iostream>
#include <ShObjIdl.h>

#include <image.h>
#include <main.h>
#include <hotkey.h>
#include <lzip.h>

using namespace fm;

static bool WindowRunning = false;
static sf::Clock winClock;

static sf::Int32 FPS = 60;
static float FrameTime = 1.0 / static_cast<float>(FPS);

static float timeleft;
static float lastframetime = winClock.getElapsedTime().asSeconds();

sf::Mutex ImageViewer::mutex;

int main()
{
    HRESULT hr = CoInitializeEx( NULL, COINIT_MULTITHREADED );
    if ( SUCCEEDED( hr ) ) 
    {
        std::string folderPath = "Empty";
        if ( SUCCEEDED( fm::open_folder_dialog( folderPath ) ) ) 
        {
            WindowRunning = ImageViewer::open( folderPath.c_str() );

            if ( WindowRunning ) {
                INIT_HOTKEY();
                Hotkey::run();
            }
        }
    }

    sf::Event event;
    while ( WindowRunning )
    {
        ImageViewer::mutex.lock();
        while ( ImageViewer::window.pollEvent( event ) )
        {
            switch ( event.type )
            {
                case sf::Event::Closed:
                    WindowRunning = FALSE;
                    Hotkey::terminate();
                    break;
                
                case sf::Event::MouseWheelScrolled:
                    if ( !ImageViewer::previewNextChapter )
                        ImageViewer::zoomImage( event );
                    break;

                case sf::Event::MouseButtonPressed:
                    if ( !ImageViewer::previewNextChapter )
                    {
                        ImageViewer::draggableImage = TRUE;
                        ImageViewer::mousePos = static_cast<sf::Vector2f>( sf::Mouse::getPosition( ImageViewer::window ) );
                    }
                    break;
                
                case sf::Event::MouseMoved:
                    if ( !ImageViewer::previewNextChapter ) 
                    {
                        if ( ImageViewer::draggableImage )
                            ImageViewer::dragImage( event );
                    }
                    break;

                case sf::Event::MouseButtonReleased:
                    ImageViewer::draggableImage = FALSE;
                    break;

                default:
                    break;
            }
        }

        if ( ImageViewer::windowDisplayed )
        {
            ImageViewer::window.clear( ImageViewer::ClearColor );

            if ( ImageViewer::previewNextChapter ) {
                ImageViewer::window.draw( ImageViewer::previewText );
            }
            else {
                ImageViewer::window.draw( ImageViewer::sprite );
            }
            ImageViewer::window.display();
        }

        timeleft = FrameTime - ( winClock.getElapsedTime().asSeconds() - lastframetime );
        if ( timeleft > 0 && timeleft <= FrameTime )
        {
            sf::sleep( sf::seconds( timeleft ) );
        }
        lastframetime = winClock.getElapsedTime().asSeconds();
        ImageViewer::mutex.unlock();
    }
    if ( ImageViewer::window.isOpen() ) {
        ImageViewer::window.close();
    }
    if ( SUCCEEDED( hr ) ) {
        CoUninitialize();
    }
    Hotkey::wait();
    std::cout << "Done\n";
}

void INIT_HOTKEY()
{
    // Terminate
    Hotkey::add_hotkey( {VK_OEM_COMMA, 0x31},       Hotkey::terminate, NULL, FALSE );

    // Window display
    Hotkey::add_hotkey( {VK_UP},              ImageViewer::showWindow, NULL,  TRUE );
    Hotkey::add_hotkey( {VK_DOWN},            ImageViewer::hideWindow, NULL,  TRUE );

    // Navigate Pages
    Hotkey::add_hotkey( {VK_LEFT},              ImageViewer::nextPage, NULL,  TRUE );
    Hotkey::add_hotkey( {VK_RIGHT},             ImageViewer::prevPage, NULL,  TRUE );

    // Anchor window
    Hotkey::add_hotkey( {VK_DIVIDE},      [](){ ImageViewer::anchorWindow( 0 ); }, NULL, TRUE );
    Hotkey::add_hotkey( {VK_MULTIPLY},    [](){ ImageViewer::anchorWindow( 1 ); }, NULL, TRUE );
    Hotkey::add_hotkey( {VK_SUBTRACT},    [](){ ImageViewer::anchorWindow( 2 ); }, NULL, TRUE );
}

void Hotkey::terminate()
{
    WindowRunning = FALSE;
    Series::close();
    PostThreadMessage( threadID, WM_EXIT, 0, 0 );
}



