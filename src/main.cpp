#include <iostream>
#include <ShObjIdl.h>

#include <image.h>
#include <main.h>
#include <hotkey.h>
#include <lzip.h>

static bool WindowRunning = false;
static sf::Clock winClock;

static UINT8 FPS = 60;
static UINT8 FrameTime = 1000 / FPS;

static sf::Int32 timeleft;
static sf::Int32 lastframetime = winClock.getElapsedTime().asMilliseconds();

int main()
{
    HRESULT hr = CoInitializeEx( NULL, COINIT_MULTITHREADED );
    if ( SUCCEEDED( hr ) ) 
    {
        std::string folderPath;
        if ( SUCCEEDED( File_M::open_folder_dialog( folderPath ) ) ) {
            std::cout << " Folder [ " << folderPath << " ] \n";
        }
    }
    // WindowRunning = ImageViewer::open( MIHON );

    // INIT_HOTKEY();
    // Hotkey::run();

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
                    ImageViewer::mousePos = { 
                        static_cast<float>( event.mouseMove.x ), 
                        static_cast<float>( event.mouseMove.y ) };
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

        if ( ImageViewer::windowDisplayed )
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
    if ( ImageViewer::window.isOpen() ) {
        ImageViewer::window.close();
    }
    if ( SUCCEEDED( hr ) ) {
        CoUninitialize();
    }
    // Hotkey::wait();
    File_M::close();
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
    File_M::close();
    PostThreadMessage( threadID, WM_EXIT, 0, 0 );
}



