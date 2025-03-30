#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include "IMUProcessor.h"
#include "DBWProcessor.h"

#include <stdio.h>
#include "GLFW/glfw3.h"


#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int, char**)
{
    IMUProcessor imuprocessor;
    DBWProcessor dbwprocessor;


    imuprocessor.parseIMUFile("data/imu.csv");
    dbwprocessor.parseDBWFile("data/dbw.csv");



    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif



    GLFWwindow* window = glfwCreateWindow(1280, 720, "IMU/DBW Data Visualization", nullptr, nullptr);
    if (window == nullptr)
        return 1;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Speed and Acceleration Trends");

        std::vector<NormalizedIMUData> imuData = imuprocessor.normalizeIMUData();
        std::vector<float> timestamps, accX, accY, accZ;

        for (const auto& data : imuData) {
            timestamps.push_back(static_cast<float>(data.timestamp));
            accX.push_back(data.acc_x);
            accY.push_back(data.acc_y);
            accZ.push_back(data.acc_z);
        }

        if (ImPlot::BeginPlot("Acceleration vs Time")) {
            ImPlot::SetNextLineStyle(ImVec4(1, 0, 0, 1), 2.0f);
            ImPlot::PlotLine("acc_x", timestamps.data(), accX.data(), static_cast<int>(timestamps.size()));

            ImPlot::SetNextLineStyle(ImVec4(0, 1, 0, 1), 2.0f);
            ImPlot::PlotLine("acc_y", timestamps.data(), accY.data(), static_cast<int>(timestamps.size()));

            ImPlot::SetNextLineStyle(ImVec4(0, 0, 1, 1), 2.0f);
            ImPlot::PlotLine("acc_z", timestamps.data(), accZ.data(), static_cast<int>(timestamps.size()));

            ImPlot::EndPlot();
        }

        std::vector<NormalizedDBWData> dbwData = dbwprocessor.normalizeDBWData();
        std::vector<float> dbwTimestamps, speed;

        for (const auto& data : dbwData) {
            dbwTimestamps.push_back(static_cast<float>(data.timestamp));
            speed.push_back(data.speed);
        }

        if (ImPlot::BeginPlot("Speed vs Time")) {
            ImPlot::SetNextLineStyle(ImVec4(1, 1, 0, 1), 2.0f);
            ImPlot::PlotLine("Speed", dbwTimestamps.data(), speed.data(), static_cast<int>(speed.size()));

            ImPlot::EndPlot();
        }

        ImGui::End();

        // Render
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
