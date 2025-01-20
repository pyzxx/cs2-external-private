#include "sdk.h"
#include "../globals/globals.h"
#include "../globals/Draw.hpp"
#include "../uitls/Vector.h"
#include "../uitls/bones.hpp"
#include <thread>

#include "../imgui/imgui_internal.h"


//get client cs2
bool Sdk::ThreadLoop()
{
	DWORD pid = mem.getPid(L"cs2.exe"); //pid 
	uintptr_t base = 0;

	if (!pid)
		return false;

	//std::this_thread::sleep_for(std::chrono::milliseconds(15));

	sdk.client = mem.GetBase(pid, L"client.dll"); // base client
	if (!sdk.client)
		return false;
	//std::cout << "client.dll " << std::hex << sdk.client << std::endl;

	return true;
}

void Sdk::LoopCheat()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(6));
		sdk.aimbot();
		sdk.Triggerbot();

	}
}




void Sdk::RenderEsp()
{
	//leituras
	uintptr_t entidade = mem.Read<uintptr_t>(sdk.client + offsets::dwEntityList);
	uintptr_t localplayer = mem.Read<uintptr_t>(sdk.client + offsets::dwLocalPlayerPawn);
	Vector3 localorigin = mem.Read<Vector3>(localplayer + offsets::m_vOldOrigin);
	uint8_t localteam = mem.Read<uint8_t>(localplayer + offsets::m_iTeamNum);
	view_matrix_t view_matrix = mem.Read<view_matrix_t>(sdk.client + offsets::dwViewMatrix);

	//percorrimento para acessar todas as entidades do jogo
	for (int i = 0; i < 64; i++)
	{
		uintptr_t listEntry = mem.Read<uintptr_t>(entidade + 0x10);
		if (listEntry == 0)
			continue;

		uintptr_t currentConroller = mem.Read<uintptr_t>(listEntry + i * 0x78);
		if (currentConroller == 0)
			continue;

		int pawnHandle = mem.Read<int>(currentConroller + offsets::m_hPlayerPawn);
		if (pawnHandle == 0)
			continue;

		uintptr_t listEntry2 = mem.Read<uintptr_t>(entidade + 0x8 * ((pawnHandle & 0x7FFF) >> 9) + 0x10);

		uintptr_t currentPawn = mem.Read<uintptr_t>(listEntry2 + 0x78 * (pawnHandle & 0x1FF));
		if (currentPawn == 0)
			continue;

		uint32_t health = mem.Read<uint32_t>(currentPawn + offsets::m_iHealth);
		uint8_t team = mem.Read<uint8_t>(currentPawn + offsets::m_iTeamNum);
		int lifeState = mem.Read<int>(currentPawn + offsets::m_lifeState);

		if (g.TeamCheck == true)
		{
			if (team == localteam)
				continue;

		}


		if (health <= 0 || lifeState != 256 || currentPawn == localplayer)
			continue;

		//acesso para pegar os ossos do boneco
		uintptr_t gamescene = mem.Read<uintptr_t>(currentPawn + offsets::m_pGameSceneNode);
		uintptr_t bonearray = mem.Read<uintptr_t>(gamescene + offsets::m_modelState + 0x80);

		//posição da entidade e osso da cabeça e do pé do boneco
		Vector3 origin = mem.Read<Vector3>(currentPawn + offsets::m_vOldOrigin); // origin da entidade
		Vector3 head = mem.Read<Vector3>(bonearray + static_cast<unsigned long long>(bones::head) * 32); // cabeça da entidade
		Vector3 feet = mem.Read<Vector3>(bonearray + static_cast<unsigned long long>(bones::right_feet) * 32); // cabeça da entidade

		// conversão em world to screen realidade 3d do jogo
		Vector3 screenFeetPos = origin.WTS(view_matrix);
		Vector3 screenHeadPos = head.WTS(view_matrix);
		Vector3 feetpos = feet.WTS(view_matrix);

		float headHeigth = (feetpos.y - screenHeadPos.y) / 8;
		float height = screenFeetPos.y - screenHeadPos.y + 15.f;
		float width = height / 1.4f;

		//bomb esp

		auto get_planted = mem.Read<uintptr_t>(sdk.client + offsets::dwPlantedC4);
		auto get_node = mem.Read<uintptr_t>(get_planted + offsets::m_pGameSceneNode);
		auto get_origin = mem.Read<Vector3>(get_node + offsets::m_vecAbsOrigin);

		auto originC4 = get_origin.WTS(view_matrix);

		//socorro
		if (g.name == true)
		{
			char Buffer[MAX_PATH]{};
			uintptr_t PlayerName = mem.Read<uintptr_t>(currentPawn + offsets::m_iszPlayerName);
			mem.ReadMemory(PlayerName, Buffer, MAX_PATH);
			ImGui::GetForegroundDrawList()->AddText({ screenHeadPos.x, screenHeadPos.y - 15 }, ImColor(255, 255, 255), Buffer);
			
			std::cout << "" << Buffer;
			
		}
	
	

		if (g.skeleton == true)
		{
			for (int i = 0; i < sizeof(boneConnections) / sizeof(boneConnections[0]); i++)
			{
				int bone1 = boneConnections[i].bone1;
				int bone2 = boneConnections[i].bone2;

				Vector3 VectorBone1 = mem.Read<Vector3>(bonearray + bone1 * 32);
				Vector3 VectorBone2 = mem.Read<Vector3>(bonearray + bone2 * 32);

				Vector3 b1 = VectorBone1.WTS(view_matrix);
				Vector3 b2 = VectorBone2.WTS(view_matrix);

				if (b1.z > 0.1f && b2.z > 0.1f) {
					desing::Line(b1.x, b1.y, b2.x, b2.y, g.skeletonColor, g.skeletonThickness);

				}


			}
		}


		ImVec4 healthColor = ImVec4(1.0f - health / 100.0f, health / 100.0f, 0.0f, 1.0f);

		if (g.e_box == true)
		{
			if (screenHeadPos.z > 0.1f && screenFeetPos.z > 0.1f) {

				desing::DrawCornerBoxWithHealthBarR(
					screenFeetPos.x - width / 2,   // Centro do box
					screenHeadPos.y - headHeigth,  // Subir o box
					width,                         // Largura do box
					height,                        // Altura do box
					g.boxColor,                    // Cor do box
					g.boxThickness,                // Espessura do box
					health,                        // Barra de vida
					healthColor                    // Cor da barra de vida
				);
			}

		}

		if (g.distance == true)
		{
			if (screenHeadPos.z > 0.1f && screenFeetPos.z > 0.1f) {
				// Desenhar a distância entre o jogador e a entidade
				desing::DrawDistance(
					localorigin.x, localorigin.y,              // Coordenadas do jogador
					origin.x, origin.y,                        // Coordenadas da entidade
					g.distancecolor,                               // Cor do texto
					screenFeetPos.x - width / 2,               // X do box
					screenHeadPos.y - headHeigth,              // Y do box
					width,                                     // Largura do box
					height                                     // Altura do box
				);
			}
		}

		Vector3 screenCenter = { screenWidth / 2.0f, 0.0f }; // centro superior
		Vector3 screenCenterinf = { screenWidth / 2.0f, screenHeight - 1.0f, 0.0f };

		if (g.lines == true)
		{
			if (screenHeadPos.z > 0.1f) {

				desing::Line(
					screenCenter.x,        // X do centro da tela
					screenCenter.y,        // Y do centro da tela
					screenHeadPos.x,       // X da cabeça da entidade
					screenHeadPos.y,       // Y da cabeça da entidade
					g.linesColor,          // Cor da linha
					g.lineThickness        // Espessura da linha
				);
			}
		}

	}
}


inline float DistTanceTo(const Vector3& v1, const Vector3& v2)
{
	return sqrt(pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2) + pow(v2.z - v1.z, 2));
}

// aim cs2 phantom store // by winscpc
void Sdk::aimbot()
{
	//resolution actual
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	float screenWidth = desktop.right; // width
	float screenHeight = desktop.bottom;// heigth

	// Verifique se a proporção está em uso
	bool useAspectRatio = mem.Read<bool>(sdk.client + offsets::m_bUseScreenAspectRatio);
	float aspectRatioX = useAspectRatio ? screenWidth / 1920.0f : 1.0f;
	float aspectRatioY = useAspectRatio ? screenHeight / 1080.0f : 1.0f;

	// Acesso para aimbot
	uintptr_t enityList = mem.Read<uintptr_t>(sdk.client + offsets::dwEntityList);
	uintptr_t listEntry = mem.Read<uintptr_t>(enityList + 0x10);
	uintptr_t PawnAddress = mem.Read<uintptr_t>(sdk.client + offsets::dwLocalPlayerPawn);
	uint8_t localteam = mem.Read<uint8_t>(PawnAddress + offsets::m_iTeamNum);
	Vector3 localorigin = mem.Read<Vector3>(PawnAddress + offsets::m_vOldOrigin);
	Vector3 localView = mem.Read<Vector3>(PawnAddress + offsets::m_vecViewOffset);
	view_matrix_t viewMatrix = mem.Read<view_matrix_t>(sdk.client + offsets::dwViewMatrix);
	Vector3 localviewangles = mem.Read<Vector3>(sdk.client + offsets::dwViewAngles);
	Vector3 localEyePos = localorigin + localView;

	// Variáveis de mouse
	float TargetX = 0.f;
	float TargetY = 0.f;
	Vector3 bestTarget;
	double distance = 1000;



	for (int i = 0; i < 64; i++)
	{

		if (listEntry == 0)
			continue;

		uintptr_t currentConroller = mem.Read<uintptr_t>(listEntry + i * 0x78);
		if (currentConroller == 0)
			continue;

		int pawnHandle = mem.Read<int>(currentConroller + offsets::m_hPlayerPawn);
		if (pawnHandle == 0)
			continue;

		uintptr_t listEntry2 = mem.Read<uintptr_t>(enityList + 0x8 * ((pawnHandle & 0x7FFF) >> 9) + 0x10);

		uintptr_t currentPawn = mem.Read<uintptr_t>(listEntry2 + 0x78 * (pawnHandle & 0x1FF));
		if (currentPawn == 0)
			continue;

		uint32_t health = mem.Read<uint32_t>(currentPawn + offsets::m_iHealth);
		uint8_t team = mem.Read<uint8_t>(currentPawn + offsets::m_iTeamNum);
		int lifeState = mem.Read<int>(currentPawn + offsets::m_lifeState);

		int SpottedByMask = mem.Read<int>(currentPawn + offsets::m_entitySpottedState + offsets::m_bSpotted);

		if (g.visibilityCheck == true)
		{
			if (!(SpottedByMask & (1 << PawnAddress)))
				continue;
		}
		


		if (g.TeamCheck == true)
		{
			if (team == localteam)
				continue;

		}


		if (health <= 0 || lifeState != 256 || currentPawn == PawnAddress)
			continue;


		//visible check


		Vector3 origin = mem.Read<Vector3>(currentPawn + offsets::m_vOldOrigin);
		Vector3 view = mem.Read<Vector3>(currentPawn + offsets::m_vecViewOffset);
		uintptr_t sceneNode = mem.Read<uintptr_t>(currentPawn + offsets::m_pGameSceneNode);
		uintptr_t boneMatrix = mem.Read<uintptr_t>(sceneNode + offsets::m_modelState + 0x80);
		Vector3 entityHead = mem.Read<Vector3>(boneMatrix + 6 * 32);
		Vector3 ScreenPos = entityHead.WTS(viewMatrix);

		double currentDistance = sqrt(pow(ScreenPos.x - screenWidth / 2, 2) + pow(ScreenPos.y - screenHeight / 2, 2));
		if (currentDistance < distance)
		{
			distance = currentDistance;
			bestTarget = ScreenPos;
		}
	}

	Vector3 ScreenPos = bestTarget; // head
	if (distance > g.Aimfov) return;

	// Ajustar a centralização com base na proporção
	float ScreenCenterX = screenWidth / 2;
	float ScreenCenterY = screenHeight / 2;

	if (ScreenPos.x != ScreenCenterX)
	{
		TargetX = (ScreenPos.x - ScreenCenterX) * aspectRatioX;
		TargetX /= (g.Aimsmoothing != 0.0f) ? g.Aimsmoothing : 1.0f;
	}

	if (ScreenPos.y != ScreenCenterY)
	{
		TargetY = (ScreenPos.y - ScreenCenterY) * aspectRatioY;
		TargetY /= (g.Aimsmoothing != 0.0f) ? g.Aimsmoothing : 1.0f;
	}

	if (g.aimbot == true)
	{
		
			if (g.AimKey != 0 && g.IsKeyDown(g.AimKey)) 
			{  // Verifica se a tecla associada a AimKey está pressionada
				mouse_event(MOUSEEVENTF_MOVE, static_cast<int>(TargetX), static_cast<int>(TargetY), NULL, NULL);
			}
		
	}
	
}



void Sdk::RenderDbg()
{
	if (g.showFov)
	{
		ImDrawList* drawList = ImGui::GetBackgroundDrawList();
		float distance = 10000000.f;
		// Obtém o viewport principal
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 viewportPos = viewport->Pos;
		ImVec2 viewportSize = viewport->Size;

		// Calcula o centro da tela manualmente
		ImVec2 center;
		center.x = viewportPos.x + (viewportSize.x * 0.5f);
		center.y = viewportPos.y + (viewportSize.y * 0.5f);

		// Obtém o tempo atual
		float time = ImGui::GetTime(); // Tempo em segundos desde o início do aplicativo

		// Calcula a cor com base no tempo
		float red = 0.5f + 0.5f * std::sin(time);   // Varia de 0.0 a 1.0
		float green = 0.5f + 0.5f * std::sin(time + 2.0f); // Varia de 0.0 a 1.0, mas deslocado no tempo
		float blue = 0.5f + 0.5f * std::sin(time + 4.0f); // Varia de 0.0 a 1.0, deslocado no tempo

		// Garante que a cor esteja no intervalo de 0.0 a 1.0


		// Define a cor atual
		ImU32 color = ImColor(red, green, blue, 1.0f); // Totalmente opaco

		// Desenha um círculo pequeno preto preenchido no centro
	
		// Desenha o círculo do FOV com a cor que muda
		drawList->AddCircle(center, g.Aimfov, color, 0, 1.f); // O 0 define a suavização padrão e 1.0f a espessura da borda
		if (distance < g.Aimfov)
			return;
	}
}

bool isHeadInCrosshair(Vector3 screenHeadPos, float crosshairX, float crosshairY, float tolerance)
{
	// A função verifica se a posição da cabeça está dentro de uma "mira" (área de tolerância)
	// Para isso, comparamos as coordenadas 2D da cabeça com a posição da mira (crosshair)

	// Tolerance permite um pequeno erro para permitir que a cabeça não precise estar exatamente na mira
	return (screenHeadPos.x >= (crosshairX - tolerance) && screenHeadPos.x <= (crosshairX + tolerance)) &&
		(screenHeadPos.y >= (crosshairY - tolerance) && screenHeadPos.y <= (crosshairY + tolerance));
}

void Sdk::Triggerbot()
{

	//leituras
	uintptr_t entidade = mem.Read<uintptr_t>(sdk.client + offsets::dwEntityList);
	uintptr_t localplayer = mem.Read<uintptr_t>(sdk.client + offsets::dwLocalPlayerPawn);
	Vector3 localorigin = mem.Read<Vector3>(localplayer + offsets::m_vOldOrigin);
	uint8_t localteam = mem.Read<uint8_t>(localplayer + offsets::m_iTeamNum);
	view_matrix_t view_matrix = mem.Read<view_matrix_t>(sdk.client + offsets::dwViewMatrix);

	//percorrimento para acessar todas as entidades do jogo
	for (int i = 0; i < 64; i++)
	{
		uintptr_t listEntry = mem.Read<uintptr_t>(entidade + 0x10);
		if (listEntry == 0)
			continue;

		uintptr_t currentConroller = mem.Read<uintptr_t>(listEntry + i * 0x78);
		if (currentConroller == 0)
			continue;

		int pawnHandle = mem.Read<int>(currentConroller + offsets::m_hPlayerPawn);
		if (pawnHandle == 0)
			continue;

		uintptr_t listEntry2 = mem.Read<uintptr_t>(entidade + 0x8 * ((pawnHandle & 0x7FFF) >> 9) + 0x10);

		uintptr_t currentPawn = mem.Read<uintptr_t>(listEntry2 + 0x78 * (pawnHandle & 0x1FF));
		if (currentPawn == 0)
			continue;

		uint32_t health = mem.Read<uint32_t>(currentPawn + offsets::m_iHealth);
		uint8_t team = mem.Read<uint8_t>(currentPawn + offsets::m_iTeamNum);
		int lifeState = mem.Read<int>(currentPawn + offsets::m_lifeState);

		auto SpottedByMask = mem.Read<uintptr_t>(currentPawn + offsets::m_entitySpottedState + offsets::m_bSpotted);
		if (g.visibilityCheck == true)
		{
			if (!(SpottedByMask & (static_cast<unsigned long long>(1) << localplayer)))
				continue;
		}


		if (g.TeamCheck == true)
		{
			if (team == localteam)
				continue;

		}


		if (health <= 0 || lifeState != 256 || currentPawn == localplayer)
			continue;

		//acesso para pegar os ossos do boneco
		uintptr_t gamescene = mem.Read<uintptr_t>(currentPawn + offsets::m_pGameSceneNode);
		uintptr_t bonearray = mem.Read<uintptr_t>(gamescene + offsets::m_modelState + 0x80);

		// Posição da entidade e osso da cabeça
		Vector3 origin = mem.Read<Vector3>(currentPawn + offsets::m_vOldOrigin); // origin da entidade
		Vector3 head = mem.Read<Vector3>(bonearray + static_cast<unsigned long long>(bones::head) * 32); // cabeça da entidade

		// Converte a posição da cabeça (3D) para a posição da tela (2D)
		Vector3 screenHeadPos = head.WTS(view_matrix);

		// Defina a posição da mira (exemplo)
		// As coordenadas do centro da tela seriam algo como:
		float crosshairX = screenWidth / 2.0f;
		float crosshairY = screenHeight / 2.0f;
		float tolerance = 10.0f;  // Um pequeno valor de tolerância para permitir um erro na mira

		if (g.trigger == true)
		{
			// Verificar se o botão lateral inferior está pressionado
			if (g.AimKey != 0 && g.IsKeyDown(g.TriggerKey)) 
			{  // Verifica se a tecla associada a AimKey está pressionada
				// Verificar se a posição da cabeça está visível e dentro da área da mira
				if (screenHeadPos.x >= 0 && screenHeadPos.x <= screenWidth && screenHeadPos.y >= 0 && screenHeadPos.y <= screenHeight) {
					// Verificar se a cabeça está dentro da mira
					if (isHeadInCrosshair(screenHeadPos, crosshairX, crosshairY, tolerance)) {
						// Chama a função de clique do mouse
						simMouseclick();
					}
				}
			}

			Sleep(0.1);
		}
	}
}