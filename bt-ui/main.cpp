#ifdef NO_CONSOLE
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

#include "utils.hpp"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

// expose our window
GLFWwindow* window = nullptr;

// pointer to opensans font from memory
extern const char* opensans_regular_font_compressed_data_base85;
const char* mainFont = opensans_regular_font_compressed_data_base85;
const float fontSize = 22; // DO NOT CHANGE

// raw icon data in memory
extern unsigned char icon_32x32_png[];
extern unsigned int icon_32x32_png_len;

void HandleShortcuts(int mods, int key, int action);
void DrawMainGui();

static void glfw_error_callback(int error, const char* description) {
    LogError("GLFW Error {}: {}", error, description);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    HandleShortcuts(mods, key, action);
}

int main(int, char**) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    window = glfwCreateWindow(1280, 720, "BTorrent", nullptr, nullptr);
    glfwSetWindowSizeLimits(window, 800, 500, GLFW_DONT_CARE, GLFW_DONT_CARE);
    if (window == nullptr)
        return 1;

    // setting up window icon for glfw
    GLFWimage images[1];
    unsigned char* logo = stbi_load_from_memory(icon_32x32_png, icon_32x32_png_len,
                                                &images[0].width, &images[0].height, NULL, 4);
    if (logo) {
        images[0].pixels = logo;
        glfwSetWindowIcon(window, 1, images);
        stbi_image_free(images[0].pixels);
    } else {
        LogError("failed to load window logo from memory");
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    io.IniFilename = NULL; // disable saving state of windows

    // Setup Dear ImGui style
    ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // set our main font from memory
    ImFontConfig fcon;
    fcon.FontDataOwnedByAtlas = false;
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(mainFont, fontSize, &fcon, 0);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Draw our main gui :)
        DrawMainGui();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0, 0, 0, 0xFF);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}