#include "render.h"
#include "overlay.h"
#include "../cheat/sdk.h"
#include <thread>
#include <iostream>
#include "../globals/globals.h"
#include <string>
#include "custom.hpp"

void Hotkey1(int* k, const ImVec2& size_arg = ImVec2(0, 0))
{
    static bool waitingforkey = false;
    if (!waitingforkey) {
        if (ImGui::Button(g.KeyNames[*k], size_arg)) // Mostra o nome da tecla atual
            waitingforkey = true;
    }
    else {
        ImGui::Button("...", size_arg); // Mostra "..." enquanto aguarda tecla
        for (int Key = 0; Key < 256; Key++) { // Verifica todas as teclas
            if (GetAsyncKeyState(Key) & 0x8000) {
                *k = Key; // Atualiza a tecla
                waitingforkey = false; // Finaliza captura
            }
        }
    }
}

void Hotkey2(int* k, const ImVec2& size_arg = ImVec2(0, 0))
{
    static bool waitingforkey = false;
    if (!waitingforkey) {
        if (ImGui::Button(g.KeyNames[*k], size_arg)) // Mostra o nome da tecla atual
            waitingforkey = true;
    }
    else {
        ImGui::Button("...", size_arg); // Mostra "..." enquanto aguarda tecla
        for (int Key = 0; Key < 256; Key++) { // Verifica todas as teclas
            if (GetAsyncKeyState(Key) & 0x8000) {
                *k = Key; // Atualiza a tecla
                waitingforkey = false; // Finaliza captura
            }
        }
    }
}

void ovcontrol() {
    while (g.Run) {
        if (GetAsyncKeyState(VK_INSERT))
            g.ShowMenu = !g.ShowMenu;

        Sleep(100);
    }
}


bool Render::loopRender()
{
	
	   

    std::thread([&]() { ovcontrol(); }).detach();

    // starta/configura o imgui
    InitMenu();

    // renderiza o menu
    BeatMenu();
    DestroyMenu();

	return true;
}


void overlayX::menu() {
    //Start the Dear ImGui frame
    static bool bools[50]{};
    static int ints[50]{};
    vector < const char* > items = { "Head", "Chest", "Body", "Legs", "Feet" };

    static float color[4] = { 1.f, 1.f, 1.f, 1.f };

    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(510, 380));

    ImGui::Begin("Hello, world!", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse); {

        auto window = GetCurrentWindow();
        auto draw = window->DrawList;
        auto pos = window->Pos;
        auto size = window->Size;
        auto style = GetStyle();

        custom.m_anim = ImLerp(custom.m_anim, 1.f, 0.03f);

        draw->AddLine(pos + ImVec2(65, 40), pos + ImVec2(size.x - 15, 40), ImColor(1.f, 1.f, 1.f, 0.05f));

        switch (custom.m_tab) {

        case 0:

            SetCursorPos(ImVec2(70, 13));
            BeginGroup();

            for (int i = 0; i < custom.rage_subtabs.size(); ++i) {

                if (custom.sub_tab(custom.rage_subtabs.at(i), custom.m_rage_subtab == i) && custom.m_rage_subtab != i)
                    custom.m_rage_subtab = i, custom.m_anim = 0.f;

                if (i != custom.rage_subtabs.size() - 1)
                    SameLine();
            }

            EndGroup();

            SetCursorPos(ImVec2(65, 50));
            BeginChild("##rage_childs", ImVec2(GetWindowWidth() - 80, GetWindowHeight() - 80)); {

                switch (custom.m_rage_subtab) {

                case 0:

                    custom.begin_child("General", ImVec2(GetWindowWidth() / 2 - GetStyle().ItemSpacing.x / 2, GetWindowHeight())); {
                        Checkbox("Aimbot Enable", &g.aimbot);
                        if (g.aimbot) {
                            SliderFloat("Smoothing", &g.Aimsmoothing, 0.4f, 10.f);
                            SliderFloat("FOV", &g.Aimfov, 0.f, 300.f);

                            // Exibir texto "Aim Key:" seguido pelo botão na mesma linha
                            Text("Aim Key:");
                            ImGui::SameLine(); // Coloca o próximo item na mesma linha
                            Hotkey1(&g.AimKey); // Botão para trocar a tecla
                        }

                        Checkbox("Show Fov", &g.showFov);
                    } custom.end_child();

                    SameLine();

                    custom.begin_child("Other", ImVec2(GetWindowWidth() / 2 - GetStyle().ItemSpacing.x / 2, GetWindowHeight())); {
                        Checkbox("Triggerbot", &g.trigger);

                        if (g.trigger) {
                            // Exibir texto "Trigger Key:" seguido pelo botão na mesma linha
                            Text("Trigger Key:");
                            ImGui::SameLine(); // Coloca o próximo item na mesma linha
                            Hotkey2(&g.TriggerKey); // Botão para trocar a tecla
                        }

                        Checkbox("Team Check", &g.TeamCheck);
                        Checkbox("Visibility Check", &g.visibilityCheck);
                       
                    } custom.end_child();

                    break;

                }

            } EndChild();

            break;

        case 1:

            SetCursorPos(ImVec2(70, 13));
            BeginGroup();

            for (int i = 0; i < custom.visuals_subtabs.size(); ++i) {

                if (custom.sub_tab(custom.visuals_subtabs.at(i), custom.m_visuals_subtab == i) && custom.m_visuals_subtab != i)
                    custom.m_visuals_subtab = i, custom.m_anim = 0.f;

                if (i != custom.visuals_subtabs.size() - 1)
                    SameLine();
            }

            EndGroup();

            SetCursorPos(ImVec2(65, 50));
            BeginChild("##visuals_childs", ImVec2(GetWindowWidth() - 80, GetWindowHeight() - 80)); {

                switch (custom.m_visuals_subtab) {

                case 0: // ESP Settings

                  
                    custom.begin_child("ESP", ImVec2(GetWindowWidth() / 2 - GetStyle().ItemSpacing.x / 2, GetWindowHeight())); {
                        // Configurações do ESP
                        ImGui::Checkbox("Skeleton", &g.skeleton);
                        ImGui::Checkbox("Box", &g.e_box);

                        ImGui::Checkbox("Lines", &g.lines);
                        ImGui::Checkbox("Distance", &g.distance);
                        ImGui::Checkbox("Name", &g.name);
                        
                    } custom.end_child();

                    SameLine();

                    custom.begin_child("ESP COLORS", ImVec2(GetWindowWidth() / 2 - GetStyle().ItemSpacing.x / 2, GetWindowHeight())); {
                        
                        // Cor para Skeleton
                        ImGui::ColorEdit4("Skeleton", (float*)&g.skeletonColor);

                        // Cor para Box
                        ImGui::ColorEdit4("Box", (float*)&g.boxColor);

                        // Cor para Lines
                        ImGui::ColorEdit4("Lines", (float*)&g.linesColor);

                        // Cor para Distance
                        ImGui::ColorEdit4("Distance",(float*)&g.distancecolor);

                        ImGui::ColorEdit4("Name", (float*)&g.NameColor);

                        Checkbox("StreamProof", &g.StreamProof);
                        if (ImGui::Button("Desinject (clean)")) {
                            g.Run = false;
                            exit(0);
                        }
                    } custom.end_child();
                    break;
                }

            } EndChild();

            break;
        }





        SetCursorPosY(0);
        custom.tab_area("##tab_area", ImVec2(50, size.y - 20), []() {

            for (int i = 0; i < custom.tabs.size(); ++i)
                if (custom.tab(custom.tabs_icons.at(i), custom.tabs.at(i), custom.m_tab == i) && custom.m_tab != i)
                    custom.m_tab = i, custom.m_anim = 0.f;

            });

        // footer
        draw->AddRectFilled(pos + ImVec2(0, size.y - 20), pos + size, ImColor(10, 10, 10), style.WindowRounding, ImDrawFlags_RoundCornersBottom);
        draw->AddText(pos + ImVec2(5, size.y - 18), GetColorU32(ImGuiCol_Text), "Private External");
        draw->AddText(pos + ImVec2(size.x - CalcTextSize("Phantom.lol").x - 5, size.y - 18), GetColorU32(ImGuiCol_Text), "Phantom.lol");

    } ImGui::End();

    PopStyleVar(2);
}

