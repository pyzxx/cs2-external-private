#include <cstddef>
#include "../imgui/imgui.h"

namespace config
{
	inline bool showwindow = true;
	inline INT currenttab;

	namespace cheat
	{
		inline bool streamproof;
		inline bool teamcheck;
		inline bool deadcheck;
		inline bool botcheck;
	}
}

struct globals {
    // System(Base)
    HWND hwnd;
    WNDCLASSEXW wc;
    RECT csRect{};
    POINT csPoint{};
    int AimKey;  // Tecla do aimbot
    int TriggerKey;
    bool Run = true;
    bool ShowMenu = true;
    int MenuKey = VK_INSERT;
    bool Crosshair = false;
    bool TeamCheck = false;
    // Aim
    bool aimbot = false;
    bool trigger = false;

    bool showFov = false;
    float Aimfov = 30.f;
    float Aimsmoothing = 0.4f; // min 1, max 5 menu 1 == semi rage // 3 or 4 legit 5 full legit 2.8f best config
    float fovSpedd;
    // Visuals
    bool visuals = false;
    bool visibilityCheck = false;
  
    int screenfov = 0;
    bool e_box = false;
    bool StreamProof = false;
    bool skeleton = false;
    bool distance = false;
    bool lines = false;
    bool name = false;
    bool bombEsp = false;
    bool healthBar = false;
    bool noflash = false;
   
    bool AimRCS;

    bool isC4Planted;



    ImVec4 linesColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);     // Branco
    ImVec4 skeletonColor = ImVec4(1.0f, 0.0f, 1.0f, 0.5f);
    ImVec4 boxColor = ImVec4(0.0f, 1.0f, 1.0f, 1.0f); // Ciano
    ImVec4 distancecolor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);      // Branco     
    ImVec4 BombEspColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);      // Branco  
    ImVec4 NameColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);      // Branco  

    // Padrão
    float lineThickness = 1;    // Espessura das linhas
    float boxThickness = 1;     // Espessura das caixas
    float healthBarThickness = 3; // Espessura da barra de saúde
    float skeletonThickness = 2;

  

    const char* KeyNames[128] = 
    {
        "NONE",
        "Mouse Left",
        "Mouse Right",
        "Cancel",
        "Middle Center",
        "MouseSide1",
        "MouseSide2",
        "",
        "Backspace",
        "Tab",
        "",
        "",
        "Clear",
        "Enter",
        "",
        "",
        "Shift",
        "Ctrl",
        "Alt",
        "Pause",
        "CapsLock",
        "",
        "",
        "",
        "",
        "",
        "",
        "Escape",
        "",
        "",
        "",
        "",
        "Space",
        "Page Up",
        "Page Down",
        "End",
        "Home",
        "Left",
        "Up",
        "Right",
        "Down",
        "",
        "",
        "",
        "Print",
        "Insert",
        "Delete",
        "",
        "0",
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        "9",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "A",
        "B",
        "C",
        "D",
        "E",
        "F",
        "G",
        "H",
        "I",
        "J",
        "K",
        "L",
        "M",
        "N",
        "O",
        "P",
        "Q",
        "R",
        "S",
        "T",
        "U",
        "V",
        "W",
        "X",
        "Y",
        "Z",
        "",
        "",
        "",
        "",
        "",
        "Numpad 0",
        "Numpad 1",
        "Numpad 2",
        "Numpad 3",
        "Numpad 4",
        "Numpad 5",
        "Numpad 6",
        "Numpad 7",
        "Numpad 8",
        "Numpad 9",
        "Multiply",
        "Add",
        "",
        "Subtract",
        "Decimal",
        "Divide",
        "F1",
        "F2",
        "F3",
        "F4",
        "F5",
        "F6",
        "F7",
        "F8",
        "F9",
        "F10",
        "F11",
        "F12",
    };

    // Função para verificar se a tecla está pressionada
    bool IsKeyDown(int VK) {
        return (GetAsyncKeyState(VK) & 0x8000) != 0;
    }

  
};

// Instância global
inline globals g;
namespace offsets
{
	//att constantemente //offsets.hpp cs2 dumper
	constexpr std::ptrdiff_t dwEntityList = 0x1A146E8;
	constexpr std::ptrdiff_t dwGlobalVars = 0x185C9B0;
	constexpr std::ptrdiff_t dwLocalPlayerPawn = 0x1868CC8;
	constexpr std::ptrdiff_t dwViewAngles = 0x1A89710;
	constexpr std::ptrdiff_t dwViewMatrix = 0x1A7F610;
	
    constexpr std::ptrdiff_t dwLocalPlayerController = 0x1A64E80;


	constexpr std::ptrdiff_t m_vecC4ExplodeSpectatePos = 0x14B8; // Vector


	constexpr std::ptrdiff_t dwWindowHeight = 0x6140CC;
	constexpr std::ptrdiff_t dwWindowWidth = 0x6140C8;
	constexpr std::ptrdiff_t dwWeaponC4 = 0x19F55F8;
    constexpr std::ptrdiff_t dwPlantedC4 = 0x1A84F90;


	 // att dificilmente // client_dll.cs cs2 dumper               https://github.com/a2x/cs2-dumper/blob/main/output/client_dll.cs
	constexpr std::ptrdiff_t m_iTeamNum = 0x3E3;
	constexpr std::ptrdiff_t m_vOldOrigin = 0x1324; //vector3
	constexpr std::ptrdiff_t m_vecViewOffset = 0xCB0;
	constexpr std::ptrdiff_t m_lifeState = 0x348;
	constexpr std::ptrdiff_t m_entitySpottedState = 0x23D0;
	constexpr std::ptrdiff_t m_bSpotted = 0x8;
	constexpr std::ptrdiff_t m_bSpottedByMask = 0xC;
	constexpr std::ptrdiff_t dwViewRender = 0x1A54D60;
	constexpr std::ptrdiff_t m_modelState = 0x170;
    constexpr std::ptrdiff_t m_vecAbsOrigin = 0xD0; // Vector
	constexpr std::ptrdiff_t m_pGameSceneNode = 0x328;
	constexpr std::ptrdiff_t m_hPlayerPawn = 0x80C;
	constexpr std::ptrdiff_t m_iHealth = 0x344;
	constexpr std::ptrdiff_t m_aimPunchAngle = 0x1574;
	constexpr std::ptrdiff_t m_hPawn = 0x62C;
	constexpr std::ptrdiff_t m_pCameraServices = 0x11E0;
	constexpr std::ptrdiff_t m_iFOV = 0x210;
	constexpr std::ptrdiff_t m_bIsScoped = 0x23D0;
	constexpr std::ptrdiff_t m_iszPlayerName = 0x660;
	constexpr std::ptrdiff_t m_sSanitizedPlayerName = 0x770;
	constexpr std::ptrdiff_t m_bUseScreenAspectRatio = 0x585; // bool
}








