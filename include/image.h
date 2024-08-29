#ifndef _IMAGE_
#define _IMAGE_

#define MIHON "F:/Backup/.Oppo/Download/.mpak/Mihon/downloads/Comick (EN)/custom_Chapter 84.5_ (Growth Process _ Volume 16 Extras)"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <filesystem>
#include <unordered_set>
#include <vector>
#include <SFML/Graphics.hpp>

namespace fs = std::filesystem;

class ImageViewer
{
public:
    inline static std::unordered_set<std::string> valid_extensions = {
        ".jpg",
        ".png",
        ".jpeg"
    };

    // Media info
    static int pageIndex;
    static fs::path image_folder;
    static std::vector<std::string> filepaths;

    // Window
    static BOOL displayWindow;
    static sf::Color ClearColor;

    static sf::RenderWindow window;
    static sf::WindowHandle windowHandle;

    static sf::View view;

    static sf::Image image;
    static sf::Texture texture;
    static sf::Sprite sprite;

    static float screen_width;
    static float screen_height;

    static float sprite_width;
    static float sprite_height;

    // Can be retrived using screen_height / texture_height
    static float resize_scale;
    static float initial_scale;

    // Mouse drag
    static BOOL draggableImage;
    static sf::Vector2f mousePos;

    static void open( const char *folder );
    static void setImage( const std::string &filename );
    
    static void hideWindow();
    static void showWindow();

    static void nextPage();
    static void prevPage();

    static void zoomImage( sf::Event &event );
    static void dragImage( sf::Event &event );

    static void keepImage();
};

#endif