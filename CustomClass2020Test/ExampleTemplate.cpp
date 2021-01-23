// ManiaPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ManiaModLoader.h"
#include "SonicMania.h"
#include <vector>
#include <ctime>
#include <algorithm>


extern "C"
{
	using namespace SonicMania;

	//SoundFX Defines from SonicMania/Data/Sounds/
	const char* SFX_MySoundFX = "PathTo/MySound.wav";
	/*----------------------------------------------*/

	bool LoadedSounds = false;
	void LoadSounds()
	{
		if (!LoadedSounds)
		{
			//Load Sounds on First Run. //Global Scope fine for most things
			LoadSoundFX(SFX_MySoundFX, Scope_Global);
			LoadedSounds = true;
		}

	}
	
	void GenericDoubleJump()
	{
		//EntityPlayer** CurrentEntity = (EntityPlayer**)(baseAddress + 0x00AA7634);
		
		static EntityPlayer **CurrentEntity = (EntityPlayer**)(baseAddress + 0x00A535AC);
		EntityPlayer* player = *CurrentEntity;
		//auto player = CurPlayer;
		//printf("in DoubleJumpMoveset \n");
		if (player->RingCount >= 50 && GetChaosEmeraldCount() >= 7)
		{
			if (PlayerControllers[player->ControllerID].Y.Press && player->Grounded != 1 && player->SuperState != SuperState_Active)
			{
				player->SuperState = SuperState_Activate;
				return;
			}
			if (PlayerControllers[player->ControllerID].Y.Press && player->Grounded != 1 && player->SuperState == SuperState_Active)
			{
				player->SuperState = SuperState_Deactivate;
				return;
			}
		}
		//StartMoveset Executed when Double Jumping
		if (player->Shield <= 1 || player->SuperState == SuperState_Active)
		{
			if (player->State == PLAYERSTATE_JUMP && (PlayerControllers[player->ControllerID].A.Press || PlayerControllers[player->ControllerID].B.Press))
			{
				printf("in DoubleJumpMoveset A Press \n");
				if (player->dword218 == 0) //The Memeory at dword218 is being used here to tell the moveset if it's already done the double jump or not. 
											// since it's not being used for anything we can use it. 
				{
					PlaySoundFXS("Global/Jump.wav");
					player->SetVelocity(player->XVelocity, -350000);
					player->dword218 = 1;
				}
			}
		}
		else
		{
			//Otherwise Run the Normal Sonic Moveset code (Allows for Shield Usage).
			typedef int func(void);
			func* RunFuct = (func*)MOVESET_SONIC;
			RunFuct();
		}
	}
	__declspec(dllexport) void OnScreenUpdate()
	{
		//Code Here Runs when the Game Updates the Screen Position.
	}
	__declspec(dllexport) void OnFrame()
	{
		if (GameState & GameState_Running && !(GameState & GameState_SoftPause) )
		{
			LoadSounds();
			EntityPlayer* players [4] = { &Player1, &Player2, &Player3, &Player4 };

			for (int i = 0; i < 4; ++i)
			{
				auto& player = *players[i];
				if (player.Character == Character_Sonic)
				{
					if (player.Moveset != GenericDoubleJump)
					{
						player.Moveset = GenericDoubleJump;
					}
					if (player.dword218 == 1 && player.Grounded)
					{
						player.dword218 = 0;
					}
				}
			}
			//Your Code runs here - Only running when game is running and not paused.
		}
	}
	__declspec(dllexport) void Init(const char* path)
	{
		char buffer[MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH, buffer);
		SetCurrentDirectoryA(path);
		// Load files here
		SetCurrentDirectoryA(buffer);
	}

	__declspec(dllexport) ModInfo ManiaModInfo = { ModLoaderVer, GameVer };

}