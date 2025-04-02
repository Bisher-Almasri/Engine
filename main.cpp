
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <iostream>

int main(int argc, char *argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  SDL_Window *window =
      SDL_CreateWindow("BGFX SDL2 Multi-Platform", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 800, 600,
                       SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
  if (!window) {
    std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  SDL_SysWMinfo wmi;
  SDL_VERSION(&wmi.version);
  if (!SDL_GetWindowWMInfo(window, &wmi)) {
    std::cerr << "SDL_GetWindowWMInfo Error: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  bgfx::PlatformData pd{};

#if defined(SDL_VIDEO_DRIVER_WAYLAND)
  pd.ndt = wmi.info.wl.display;
  pd.nwh = (void *)wmi.info.wl.surface;
  bgfx::Init bgfxInit;
  bgfxInit.type = bgfx::RendererType::Vulkan;
#elif defined(SDL_VIDEO_DRIVER_X11)
  pd.ndt = wmi.info.x11.display;
  pd.nwh = (void *)wmi.info.x11.window;
  bgfx::Init bgfxInit;
  bgfxInit.type = bgfx::RendererType::OpenGL;
#elif defined(_WIN32)
  pd.nwh = wmi.info.win.window;
  bgfx::Init bgfxInit;
  bgfxInit.type = bgfx::RendererType::Direct3D11;
#elif defined(__APPLE__)
  pd.nwh = wmi.info.cocoa.window;
  bgfx::Init bgfxInit;
  bgfxInit.type = bgfx::RendererType::Metal;
#else
#error "Unsupported platform"
#endif

  bgfxInit.platformData = pd;
  bgfxInit.resolution.width = 800;
  bgfxInit.resolution.height = 600;
  bgfxInit.resolution.reset = BGFX_RESET_VSYNC;

  if (!bgfx::init(bgfxInit)) {
    std::cerr << "Failed to initialize BGFX" << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f,
                     0);
  bgfx::setViewRect(0, 0, 0, 800, 600);

  bool running = true;
  SDL_Event event;
  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
    }

    bgfx::touch(0);
    bgfx::frame();
  }

  bgfx::shutdown();
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
