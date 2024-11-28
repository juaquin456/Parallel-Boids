//
// Created by juaquin on 11/27/24.
//

#ifndef PARALLEL_BOIDS_SRC_GUI_H_
#define PARALLEL_BOIDS_SRC_GUI_H_

#include <SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "Boid.h"

class GUI {
  bool running;
  SDL_Window* window;
  SDL_Renderer* renderer;
  ImGuiIO* io;
 public:
  int init() {
    running = true;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return -1;
    window = SDL_CreateWindow("Boids", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
      SDL_Quit();
      return -1;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
      SDL_DestroyWindow(window);
      SDL_Quit();
      return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    return 0;
  }
  void destroy() {
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }
  void draw(Conditions& c, bool& is_parallel) {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("info");
    ImGui::Text("%.1f FPS", io->Framerate);
    ImGui::SliderFloat("Vision", &c.visual_radius, 1.f, 100.f);
    ImGui::Checkbox("Parallel?", &is_parallel);
    ImGui::End();
    ImGui::Render();



    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
  }
  void process_event() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) running = false;
      ImGui_ImplSDL2_ProcessEvent(&e);
    }
  }

  [[nodiscard]] bool is_running() const {
    return running;
  }

  SDL_Renderer* get_renderer() {
    return renderer;
  }
};


#endif //PARALLEL_BOIDS_SRC_GUI_H_
