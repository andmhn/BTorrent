#include <thread>
#include <vector>

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "nfd.hpp"
#include "nfd_glfw3.h"
#include "torrent_metadata.hpp"

extern GLFWwindow* window;

// our state
static struct State {
  bool useDarkTheme = false;
  std::optional<bt::TorrentMetadata> selectedTorrent = {};
} state;

static void SelectTorrentFile();

// TEST: spawning popup window
static void DrawTorrentPreview() {
  if (state.selectedTorrent.has_value()) {
    ImGui::OpenPopup("Torrent Info?");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Torrent Info?", NULL,
                               ImGuiWindowFlags_AlwaysAutoResize)) {
      bt::TorrentMetadata& torr = state.selectedTorrent.value();
      ImGui::Text("name: %s", torr.name().c_str());
      ImGui::Separator();
      ImGui::TextWrapped("Comment: %s", torr.comment().value_or("").c_str());
      ImGui::Separator();
      ImGui::Text("createdBy: %s", torr.createdBy().value_or("").c_str());
      ImGui::Separator();
      ImGui::Text("mainAnnounce: %s", torr.mainAnnounce().value_or("").c_str());
      ImGui::Separator();
      ImGui::Text("infoHash: %s", torr.infoHash().c_str());
      ImGui::Separator();

      if (ImGui::Button("OK", ImVec2(120, 0))) {
        ImGui::CloseCurrentPopup();
        state.selectedTorrent = {};
      }
      ImGui::SetItemDefaultFocus();
      ImGui::SameLine();
      if (ImGui::Button("Cancel", ImVec2(120, 0))) {
        ImGui::CloseCurrentPopup();
        state.selectedTorrent = {};
      }
      ImGui::EndPopup();
    }
  }
}

void DrawMainGui() {
  // Add menu bar flag and disable everything else
  ImGuiWindowFlags flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings |
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar;

  ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
  ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

  ImGui::Begin("root", 0, flags);

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Open", "Ctrl+O")) {
        SelectTorrentFile();
      }
      if (ImGui::MenuItem("Exit", "Alt+F4")) {
        exit(0);  // TODO: exit gracefully
      }

      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View")) {
      if (ImGui::MenuItem("toggle dark mode", NULL, state.useDarkTheme)) {
        state.useDarkTheme = !state.useDarkTheme;
        if (state.useDarkTheme) ImGui::StyleColorsDark();
        if (!state.useDarkTheme) ImGui::StyleColorsLight();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  if (ImGui::Button("select torrent file!")) {
    SelectTorrentFile();
  }

  ImGui::TextWrapped("Application average %.3f ms/frame (%.1f FPS)",
                     1000.0f / ImGui::GetIO().Framerate,
                     ImGui::GetIO().Framerate);
  DrawTorrentPreview();
  ImGui::End();
}

void HandleShortcuts(int mods, int key) {
  if (mods == GLFW_MOD_CONTROL && key == GLFW_KEY_O) {
    puts("[Info]: CTRL-O  Pressed!");
    SelectTorrentFile();
  }
  if (mods == GLFW_MOD_ALT && key == GLFW_KEY_F4) {
    puts("[Info]: ALT-F4  Pressed! Exiting");
    exit(0);
  }
}

static void _SelectTorrentFile() {
  NFD_Init();
  nfdu8char_t* outPath;
  nfdu8filteritem_t filters[1] = {{"Torrent File", "torrent"}};
  nfdopendialogu8args_t args = {0};
  args.filterList = filters;
  args.filterCount = 1;
  // attach it to our root glfw window
  NFD_GetNativeWindowFromGLFWWindow(window, &args.parentWindow);

  nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);

  if (result == NFD_OKAY) {
    printf("[Info]: Successfully selected file: %s\n", outPath);
    state.selectedTorrent = bt::torrent_parser::ParseFromFile(outPath);
    NFD_FreePathU8(outPath);

  } else if (result == NFD_CANCEL) {
    puts("[Info]: User pressed cancel.");
  } else {
    fprintf(stderr, "[Error]: %s\n", NFD_GetError());
  }
  NFD_Quit();
}

void SelectTorrentFile() { new std::thread(_SelectTorrentFile); }