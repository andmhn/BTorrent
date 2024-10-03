#include <thread>
#include <vector>

#include "misc.hpp"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
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

constexpr size_t allignedStart = 150;
constexpr ImVec2 popupSize(800, 500);

// wrap comment to scrollable
static void _DisplayComments() {
    bt::TorrentMetadata& torr = state.selectedTorrent.value();
    ImGui::Text("comment");
    ImGui::SameLine(allignedStart);

    // calculate possible lines
    size_t lines = torr.comment().value_or("").size() / 80;

    // if line greater than 3 enable scrolling
    if (lines > 3) {
        ImGui::BeginChild("commentChild", ImVec2(ImGui::GetContentRegionAvail().x, 80),
                          ImGuiChildFlags_None, 0);
        ImGui::TextWrapped("%s", torr.comment().value_or("").c_str());
        ImGui::EndChild();
    } else {
        ImGui::TextWrapped("%s", torr.comment().value_or("").c_str());
    }
}

static void _DisplayFiles() {
    bt::TorrentMetadata& torr = state.selectedTorrent.value();
    ImGui::Text("files");
    ImGui::SameLine(allignedStart);

    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                                   ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersInnerV;
    // scroll after 8 line
    ImVec2 outer_size = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 8);

    // 2 column table with filename and size
    if (ImGui::BeginTable("filesTable", 2, flags, outer_size)) {
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
        ImGui::TableSetupColumn("File Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        // use clipper to only draw what is visible
        ImGuiListClipper clipper;

        clipper.Begin(static_cast<int>(torr.files().size()));
        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::TextWrapped("%s", torr.files()[row].GetRelativePathAsString().c_str());

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.3f MB", torr.files()[row].size / float(1024 * 1024));
            }
        }
        ImGui::EndTable();
    }
}

// spawning popup window
// TODO: resize to content
static void _DrawTorrentPreview() {
    bt::TorrentMetadata& torr = state.selectedTorrent.value();

    ImGui::OpenPopup(torr.name().c_str());
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(popupSize);

    if (ImGui::BeginPopupModal(torr.name().c_str(), NULL, ImGuiWindowFlags_NoResize)) {
        {
            ImGui::Text("size");
            ImGui::SameLine(allignedStart);
            ImGui::Text("%lld MB", (torr.piecesCount() * torr.pieceLength()) / (1024 * 1024));
            ImGui::Separator();

            ImGui::Text("infoHash");
            ImGui::SameLine(allignedStart);
            ImGui::TextUnformatted(torr.infoHash().c_str());
            ImGui::Separator();

            // TODO: convert to readable date
            ImGui::Text("date");
            ImGui::SameLine(allignedStart);
            ImGui::Text("%lld", torr.creationDate().value_or(0));
            ImGui::Separator();

            ImGui::Text("created by");
            ImGui::SameLine(allignedStart);
            ImGui::TextUnformatted(torr.createdBy().value_or("").c_str());
            ImGui::Separator();

            _DisplayComments();
            ImGui::Separator();

            _DisplayFiles();
        }
        ImGui::SetCursorPosY(popupSize.y - (2 * ImGui::GetStyle().IndentSpacing));
        if (ImGui::Button("Start", ImVec2(120, 0))) {
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

void DrawMainGui() {
    // Add menu bar flag and disable everything else
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
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
                exit(0); // TODO: exit gracefully
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("toggle dark mode", NULL, state.useDarkTheme)) {
                state.useDarkTheme = !state.useDarkTheme;
                if (state.useDarkTheme)
                    ImGui::StyleColorsDark();
                if (!state.useDarkTheme)
                    ImGui::StyleColorsLight();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (ImGui::Button("select torrent file!")) {
        SelectTorrentFile();
    }
    if (state.selectedTorrent.has_value()) {
        _DrawTorrentPreview();
    }

    ImGui::TextWrapped("Application average %.3f ms/frame (%.1f FPS)",
                       1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();
}

void HandleShortcuts(int mods, int key, int action) {
    if (mods == GLFW_MOD_CONTROL && key == GLFW_KEY_O && action == GLFW_RELEASE) {
        LogTrace("CTRL-O  Pressed and Released!");
        SelectTorrentFile();
    }
    if (mods == GLFW_MOD_ALT && key == GLFW_KEY_F4) {
        LogTrace("ALT-F4  Pressed! Exiting...");
        exit(0);
    }
}

static void _SelectTorrentFile() {
    // forbid re-selecting
    if (state.selectedTorrent.has_value()) {
        LogWarning("There's already a file selected. Skipping...");
        return;
    }

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
        LogTrace("Successfully selected file: {}", outPath);
        try {
            state.selectedTorrent = bt::torrent_parser::ParseFromFile(outPath);
        } catch (std::exception& e) {
            LogError("{}  error: {}", outPath, e.what());
        }
        NFD_FreePathU8(outPath);

    } else if (result == NFD_CANCEL) {
        LogTrace("User pressed cancel.");
    } else {
        LogError(NFD_GetError());
    }
    NFD_Quit();
}

void SelectTorrentFile() {
    new std::thread(_SelectTorrentFile);
}
