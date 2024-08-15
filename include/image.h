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
    inline static BOOL displayWindow = TRUE;
    inline static UINT8 R = 50, G = 50, B = 50, A = 255;

    inline static sf::Color ClearColor = sf::Color( R,G,B,A );
    inline static std::unordered_set<std::string> valid_extensions = {
        ".jpg",
        ".png",
        ".jpeg"
    };
    inline static UINT pageIndex = 0;
    inline static fs::path image_folder;
    inline static std::vector<std::string> filepaths;

    inline static sf::RenderWindow window;
    inline static sf::WindowHandle windowHandle;

    inline static sf::Image image;
    inline static sf::Texture texture;
    inline static sf::Sprite sprite;

    // Can be retrived using screen_height / texture_height
    inline static float resize_scale = 0.392;
    inline static float default_scale = resize_scale;

    static void open( const char *folder );
    static void setImage( const std::string &filename );
    
    static void hideWindow();
    static void showWindow();

    static void nextPage();
    static void prevPage();

    static void zoomImage( sf::Event &event );
};

#endif