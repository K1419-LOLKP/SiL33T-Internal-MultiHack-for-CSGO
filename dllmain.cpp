#include "pch.h"
#include <windows.h>
#include <stdio.h>
#include <dos.h>
#include "tlhelp32.h"
#include <iostream>
#include "assert.h"
#include <string>

#include "Entites.h"
#include "Offsets.h"
#include "GlowStructure.h"
#include "PredefinedFunc.h"
#include "Weapons.h"
#include "Physics.h"
#include "Hooks.h"
#include "SDK.h"

#define mepointer 0xE44FBCD0
using namespace std;

#pragma region globals
Entites playerList[64];
Entites me;
Entites inCH;
Weapons weapArr[1024];
Angle aimPunch;
Angle viewPunch;
Angle aimVel;
Angle targetAngle;
Angle oldAimPunch;
Angle zeroAngle;
IEngineTrace* engineTracePTR;

intptr_t clientAdd;
intptr_t engineAdd;
intptr_t glowManager;
intptr_t boneManager;
intptr_t clientState;
intptr_t curWeapAdd;
intptr_t curWeaponEntity;
void* clientMode;

short index;
short indexHook = 0;
short teamBuf;
short inCHNum;
short weaponNum;

bool doTrig;
bool doRCS;
bool RCS = true;
bool fire;
bool idle;

int actWeap = 0;
int XUID = 0;
int playerID;
int closestID = 0;
#pragma endregion

#pragma region redeffunc
void fireAShot()
{
    *(int*)(clientAdd + dwForceAttack) = 5;
    preciseDelay(0.02);
    *(int*)(clientAdd + dwForceAttack) = 4;
    preciseDelay(0.02);
}

enum ClientFrameStage_t
{
    FRAME_UNDEFINED = -1,
    FRAME_START,
    FRAME_NET_UPDATE_START,
    FRAME_NET_UPDATE_POSTDATAUPDATE_START,
    FRAME_NET_UPDATE_POSTDATAUPDATE_END,
    FRAME_NET_UPDATE_END,
    FRAME_RENDER_START,
    FRAME_RENDER_END
};


typedef void(__thiscall* FrameStageNotify)(void* _this, ClientFrameStage_t curStage);
typedef bool(__thiscall* fCreateMove)(void* _this, float, UserCmd_t*);
typedef void* (__cdecl* tCreateInterface)(const char* name, int* returnCode);
typedef void* (__cdecl* traceRay)(const char* name, int* returnCode);

FrameStageNotify oFrameStageNotify;
fCreateMove oCreateMove;
tCreateInterface engineInterface;

void* GetInterface()
{
    HMODULE buf = GetModuleHandle(L"client.dll");

    if (buf != NULL)
    {
        tCreateInterface CreateInterface = (tCreateInterface)GetProcAddress(buf, "CreateInterface");
        int returnCode = 0;
        void* xinterface = CreateInterface("VClient018", &returnCode);

        return xinterface;
    }

    else
    {
        cout << "client handle fail" << endl;
        return(NULL);
    }
}

void* GetInterface(tCreateInterface fn, const char* name)
{
    return (fn)(name, 0);
}


bool __fastcall hkCreateMove(void* thisptr, void* edx, float fl_input_sample_time, UserCmd_t* cmd)
{
    CGameTrace trace;
    CTraceFilter filter;

    if (playerList[indexHook].health > 0 && me.addressInMem != NULL && playerList[indexHook].addressInMem != NULL)
    {
        me.headPos = *(vec3*)(me.addressInMem + m_vecOrigin);
        me.headPos.z += *(float*)(me.addressInMem + m_vecViewOffset + 0x8);

        me.headPos.z += 25.0f;

        me.headPos.Init(me.headPos.x, me.headPos.y, me.headPos.z);

        playerList[indexHook].boneIndex = *(intptr_t*)(playerList[indexHook].addressInMem + m_dwBoneMatrix);

        if (playerList[indexHook].boneIndex != NULL)
        {
            playerList[indexHook].headPos.Init(*(float*)(playerList[indexHook].boneIndex + 0x30 * 8 + 0x0C), *(float*)(playerList[indexHook].boneIndex + 0x30 * 8 + 0x1C), *(float*)(playerList[indexHook].boneIndex + 0x30 * 8 + 0x2C));
        }

        playerList[indexHook].ray.Init(me.headPos, playerList[index].headPos);
        filter.pSkip = (void*)dwLocalPlayer;

        engineTracePTR->TraceRay(playerList[indexHook].ray, CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_DEBRIS | CONTENTS_HITBOX, NULL, &trace);

        me.headPos.z -= 25.0f;

        if (trace.hit_entity != NULL)
        {
            //playerList[indexHook].visible = (trace.hit_entity == (void*)playerList[indexHook].addressInMem);

            playerList[indexHook].special = false;

            if (indexHook == 2)
            {
                playerList[indexHook].special = true;
            }

        }
    }    

    indexHook++;

    if (indexHook > 63)
    {
        indexHook = 0;
    }

    if (RCS)
    {
        if (clientAdd != NULL && me.addressInMem != NULL && me.shotsFired > 0 && cmd != NULL)
        {
            //cout << "Sunbtracting" << endl;
            aimPunch = *(Angle*)(me.addressInMem + m_aimPunchAngle);
            aimPunch.Pitch *= 2.0f;
            aimPunch.Yaw *= 2.0f;

            cmd->viewangles.Pitch -= (aimPunch.Pitch);
            cmd->viewangles.Yaw -= (aimPunch.Yaw);
            cmd->viewangles.Roll = 0.0f;

            oldAimPunch = aimPunch;

            cmd->viewangles = normalize(cmd->viewangles);

            return false;
        }

        else
        {
            return(true);
        }
    }

    else
    {
        return(true);
    }
    
}

void __fastcall hkFrameStageNotify(void* _this, void* edx, ClientFrameStage_t curStage)
{
    //null out the recoil and viewpunch vectors
    if (me.addressInMem != NULL && clientState != NULL)
    {
        aimPunch = *(Angle*)(me.addressInMem + m_aimPunchAngle);
        viewPunch = *(Angle*)(me.addressInMem + m_viewPunchAngle);
        me.viewAngle = *(Angle*)(clientState + dwClientState_ViewAngles);

        *(Angle*)(me.addressInMem + m_viewPunchAngle) = zeroAngle;

        if (RCS)
        {
            *(Angle*)(me.addressInMem + m_aimPunchAngle) = zeroAngle;
        }         
    }

    oFrameStageNotify(_this, curStage);

    if (me.addressInMem != NULL)
    {
        *(Angle*)(me.addressInMem + m_viewPunchAngle) = viewPunch;

        if (RCS)
        {
            *(Angle*)(me.addressInMem + m_aimPunchAngle) = aimPunch;
        }       
    }
}

#pragma endregion

int main()
{
#pragma region weaponDeclaration
#pragma region TKnifeChanged  
    weapArr[515].skinNumber = 570;
    weapArr[515].doAim = false;
    weapArr[515].doTrigger = false;
    weapArr[515].viewModel = 507;
    weapArr[515].isKnife = true;
    weapArr[515].doRCS = false;
    //weapArr[1].custName = "Heavy Hand Cannon";
    weapArr[515].skinCondition = 0.001f;
    weapArr[515].statTrak = -1;
#pragma endregion

#pragma region TKnife  
    weapArr[59].skinNumber = 0;
    weapArr[59].doAim = false;
    weapArr[59].doTrigger = false;
    weapArr[59].viewModel = 507;
    weapArr[59].isKnife = true;
    weapArr[59].doRCS = false;
    //weapArr[1].custName = "Heavy Hand Cannon";
    weapArr[59].skinCondition = 0.001f;
    weapArr[59].statTrak = -1;
#pragma endregion

#pragma region DesertEagle  
    weapArr[1].skinNumber = 397;
    weapArr[1].doAim = false;
    weapArr[1].doTrigger = true;
    weapArr[1].doRCS = false;
    weapArr[1].custName = "Heavy Hand Cannon";
    weapArr[1].skinCondition = 0.001f;
    weapArr[1].statTrak = 1337;
#pragma endregion


#pragma region AWP
    weapArr[9].skinNumber = 1026;
    weapArr[9].doAim = false;
    weapArr[9].doTrigger = true;
    weapArr[9].doRCS = false;
    weapArr[9].fireWhenMoving = false;
    weapArr[9].fireWhenUnscoped = false;
    //weapArr[9].custName = "The Lightning Bolt";
    weapArr[9].skinCondition = 0.001f;
    weapArr[9].statTrak = -1;
#pragma endregion


#pragma region XM1014 
    weapArr[25].doAim = false;
    weapArr[25].doTrigger = true;
    weapArr[25].doRCS = false;
    //weapArr[25].custName = "Super Shotgun";
    weapArr[25].skinCondition = 0.001f;
    weapArr[25].skinNumber = 970;
    weapArr[25].statTrak = 1337;
#pragma endregion


#pragma region tec9
    weapArr[30].doAim = false;
    weapArr[30].doTrigger = true;
    weapArr[30].doRCS = false;
    //weapArr[30].custName = "REKT-9";
    weapArr[30].skinCondition = 0.001f;
    weapArr[30].skinNumber = 889;
    weapArr[30].statTrak = 1337;
#pragma endregion

#pragma region Zeus
    weapArr[31].doAim = false;
    weapArr[31].doTrigger = true;
    weapArr[31].doRCS = false;
    //weapArr[30].custName = "REKT-9";
    weapArr[31].skinCondition = 0.001f;
    weapArr[31].skinNumber = 0;
    weapArr[31].statTrak = -1;
#pragma endregion

#pragma region Five-seveN
    weapArr[3].doAim = false;
    weapArr[3].doTrigger = true;
    weapArr[3].doRCS = false;
    //weapArr[3].custName = "A Quality Meme";
    weapArr[3].skinCondition = 0.001f;
    weapArr[3].skinNumber = 837;
    weapArr[3].statTrak = 1337;
#pragma endregion


#pragma region G18 
    weapArr[4].doAim = false;
    weapArr[4].doTrigger = true;
    weapArr[4].doRCS = false;
    // weapArr[4].custName = "Water Gun";
    weapArr[4].skinCondition = 0.001f;
    weapArr[4].skinNumber = 963;
    weapArr[4].statTrak = 1337;
#pragma endregion


#pragma region SSG08
    weapArr[40].doAim = false;
    weapArr[40].doTrigger = true;
    weapArr[40].doRCS = false;
    weapArr[9].fireWhenMoving = false;
    //weapArr[40].custName = "AWP Made In China";
    weapArr[40].skinCondition = 0.001f;
    weapArr[40].skinNumber = 554;
    weapArr[40].statTrak = 1337;
#pragma endregion


#pragma region P250
    weapArr[36].doAim = false;
    weapArr[36].doTrigger = true;
    weapArr[36].doRCS = false;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[36].skinCondition = 0.001f;
    weapArr[36].skinNumber = 258;
    weapArr[36].statTrak = 1337;
#pragma endregion


#pragma region USPS
    weapArr[61].doAim = false;
    weapArr[61].doTrigger = true;
    weapArr[61].doRCS = false;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[61].skinCondition = 0.001f;
    weapArr[61].skinNumber = 504;
    weapArr[61].statTrak = 1337;
#pragma endregion

#pragma region P2000
    weapArr[32].doAim = false;
    weapArr[32].doTrigger = true;
    weapArr[32].doRCS = false;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[32].skinCondition = 0.001f;
    weapArr[32].skinNumber = 894;
    weapArr[32].statTrak = 1337;
#pragma endregion

#pragma region SCAR
    weapArr[38].doAim = false;
    weapArr[38].doTrigger = true;
    weapArr[38].doRCS = false;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[38].skinCondition = 0.001f;
    weapArr[38].skinNumber = 312;
    weapArr[38].statTrak = 1337;
#pragma endregion


#pragma region G3SG1
    weapArr[11].doAim = false;
    weapArr[11].doTrigger = true;
    weapArr[11].doRCS = false;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[11].skinCondition = 0.001f;
    weapArr[11].skinNumber = 628;
    weapArr[11].statTrak = 1337;
#pragma endregion


#pragma region DualBerretas
    weapArr[2].doAim = false;
    weapArr[2].doTrigger = true;
    weapArr[2].doRCS = false;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[2].skinCondition = 0.001f;
    weapArr[2].skinNumber = 903;
    weapArr[2].statTrak = 1337;
#pragma endregion

#pragma region SawedOff
    weapArr[29].doAim = false;
    weapArr[29].doTrigger = true;
    weapArr[29].doRCS = false;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[29].skinCondition = 0.001f;
    weapArr[29].skinNumber = 517;
    weapArr[29].statTrak = 1337;
#pragma endregion

#pragma region Nova
    weapArr[35].doAim = false;
    weapArr[35].doTrigger = true;
    weapArr[35].doRCS = false;
    //weapArr1].custName = "Heavy Hand Cannon";
    weapArr[35].skinCondition = 0.001f;
    weapArr[35].skinNumber = 62;
    weapArr[35].statTrak = 1337;
#pragma endregion

#pragma region MAG7
    weapArr[27].doAim = false;
    weapArr[27].doTrigger = true;
    weapArr[27].doRCS = false;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[27].skinCondition = 0.001f;
    weapArr[27].skinNumber = 703;
    weapArr[27].statTrak = 1337;
#pragma endregion

#pragma region AK47
    weapArr[7].doAim = true;
    weapArr[7].doTrigger = false;
    weapArr[7].doRCS = true;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[7].skinCondition = 0.001f;
    weapArr[7].skinNumber = 44;
    weapArr[7].seed = 1;
    weapArr[7].statTrak = 1337;
#pragma endregion

#pragma region FAMAS
    weapArr[10].doAim = true;
    weapArr[10].doTrigger = false;
    weapArr[10].doRCS = true;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[10].skinCondition = 0.001f;
    weapArr[10].skinNumber = 626;
    weapArr[10].statTrak = 1337;
#pragma endregion

#pragma region Galil
    weapArr[13].doAim = true;
    weapArr[13].doTrigger = false;
    weapArr[13].doRCS = true;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[13].skinCondition = 0.001f;
    weapArr[13].skinNumber = 398;
    weapArr[13].statTrak = 1337;
#pragma endregion

#pragma region M249
    weapArr[14].doAim = false;
    weapArr[14].doTrigger = false;
    weapArr[14].doRCS = true;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[14].skinCondition = 0.001f;
    weapArr[14].skinNumber = 547;
    weapArr[14].statTrak = 1337;
#pragma endregion

#pragma region M4A4
    weapArr[16].doAim = true;
    weapArr[16].doTrigger = false;
    weapArr[16].doRCS = true;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[16].skinCondition = 0.001f;
    weapArr[16].skinNumber = 512;
    weapArr[16].statTrak = 1337;
#pragma endregion

#pragma region AUG
    weapArr[8].doAim = true;
    weapArr[8].doTrigger = false;
    weapArr[8].doRCS = true;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[8].skinCondition = 0.001f;
    weapArr[8].skinNumber = 845;
    weapArr[8].statTrak = 1337;
#pragma endregion

#pragma region SG553
    weapArr[39].doAim = true;
    weapArr[39].doTrigger = false;
    weapArr[39].doRCS = true;
    weapArr[39].holdScope = true;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[39].skinCondition = 0.001f;
    weapArr[39].skinNumber = 613;
    weapArr[39].statTrak = 1337;
#pragma endregion

#pragma region MAC10
    weapArr[17].doAim = true;
    weapArr[17].doTrigger = false;
    weapArr[17].doRCS = true;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[17].skinCondition = 0.001f;
    weapArr[17].skinNumber = 947;
    weapArr[17].statTrak = 1337;
#pragma endregion

#pragma region P90
    weapArr[19].doAim = true;
    weapArr[19].doTrigger = false;
    weapArr[19].doRCS = true;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[19].skinCondition = 0.001f;
    weapArr[19].skinNumber = 156;
    weapArr[19].statTrak = 1337;
#pragma endregion

#pragma region MP5SD
    weapArr[23].doAim = true;
    weapArr[23].doTrigger = false;
    weapArr[23].doRCS = true;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[23].skinCondition = 0.001f;
    weapArr[23].skinNumber = 846;
    weapArr[23].statTrak = 1337;
#pragma endregion

#pragma region UMP45
    weapArr[24].doAim = true;
    weapArr[24].doTrigger = false;
    weapArr[24].doRCS = true;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[24].skinCondition = 0.001f;
    weapArr[24].skinNumber = 556;
    weapArr[24].statTrak = 1337;
#pragma endregion

#pragma region PP-Bizon
    weapArr[26].doAim = true;
    weapArr[26].doTrigger = false;
    weapArr[26].doRCS = true;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[26].skinCondition = 0.001f;
    weapArr[26].skinNumber = 973;
    weapArr[26].statTrak = 1337;
#pragma endregion

#pragma region Negev
    weapArr[28].doAim = false;
    weapArr[28].doTrigger = false;
    weapArr[28].doRCS = true;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[28].skinCondition = 0.001f;
    weapArr[28].skinNumber = 514;
    weapArr[28].statTrak = 1337;
#pragma endregion

#pragma region MP7
    weapArr[33].doAim = true;
    weapArr[33].doTrigger = false;
    weapArr[33].doRCS = true;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[33].skinCondition = 0.001f;
    weapArr[33].skinNumber = 696;
    weapArr[33].statTrak = 1337;
#pragma endregion

#pragma region MP9
    weapArr[34].doAim = true;
    weapArr[34].doTrigger = false;
    weapArr[34].doRCS = true;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[34].skinCondition = 0.001f;
    weapArr[34].skinNumber = 734;
    weapArr[34].statTrak = -1;
#pragma endregion

#pragma region M4A1S
    weapArr[60].doAim = true;
    weapArr[60].doTrigger = false;
    weapArr[60].doRCS = true;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[60].skinCondition = 0.001f;
    weapArr[60].skinNumber = 984;
    weapArr[60].statTrak = 1337;
#pragma endregion

#pragma region CZ75
    weapArr[63].doAim = true;
    weapArr[63].doTrigger = false;
    weapArr[63].doRCS = true;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[63].skinCondition = 0.001f;
    weapArr[63].skinNumber = 270;
    weapArr[63].statTrak = 1337;
#pragma endregion

#pragma region R8
    weapArr[64].doAim = true;
    weapArr[64].doTrigger = false;
    weapArr[64].doRCS = true;
    //weapArrr1].custName = "Heavy Hand Cannon";
    weapArr[64].skinCondition = 0.001f;
    weapArr[64].skinNumber = 683;
    weapArr[64].statTrak = 1337;
#pragma endregion

#pragma endregion
    zeroAngle.Pitch = 0.0f;
    zeroAngle.Yaw = 0.0f;
    zeroAngle.Roll = 0.0f;

    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

    system("cls");

    cout << "V3RY L33T STRING" << endl;

    clientAdd = (intptr_t)GetModuleHandle(L"client.dll");
    engineAdd = (intptr_t)GetModuleHandle(L"engine.dll");

    if (clientAdd == NULL || engineAdd == NULL)
    {
        while (true);
    }

    engineInterface = (tCreateInterface)GetProcAddress((HMODULE)engineAdd, "CreateInterface");
    engineTracePTR = (IEngineTrace*)GetInterface(engineInterface, "EngineTraceClient004");

    if (engineTracePTR == NULL)
    {
        cout << "engineTrace ptr FAIL EXPLOSION" << endl;
        while (true);
    }

    void* Client = GetInterface();
    if (Client != NULL)
    {
        DWORD* clientVTable = (*reinterpret_cast<DWORD**>(Client));
        if (clientVTable != NULL)
        {
            oFrameStageNotify = (FrameStageNotify)TrampHook32((char*)clientVTable[37], (char*)hkFrameStageNotify, 9);
            
            IClientMode* clientMode = **(IClientMode***)((*(uintptr_t**)(Client))[10] + 0x5);
            oCreateMove = (fCreateMove)TrampHook32((char*)((*(void***)clientMode)[24]), (char*)hkCreateMove, 9);
            //Works.
        }

        else
        {
            cout << "Virtual Table Failure!" << endl;
        }
    }

    else
    {
        cout << "Interface Creator Failure! BOOHOO" << endl;
    }

    cout << "Successful entry!" << endl;
    idle = false;
    
    while (!GetAsyncKeyState(VK_END))
    {
        glowManager = *(intptr_t*)(clientAdd + dwGlowObjectManager);
        clientState = *(intptr_t*)(engineAdd + dwClientState);

        me.addressInMem = *(intptr_t*)(clientAdd + dwLocalPlayer);

        if (me.addressInMem != NULL && !(idle) && clientState != NULL)
        {
            teamBuf = *(int*)(me.addressInMem + m_iTeamNum);
            playerID = *(int*)(clientState + dwClientState_GetLocalPlayer);
            me.viewAngle = *(Angle*)(clientState + dwClientState_ViewAngles);
            me.health = *(int*)(me.addressInMem + m_iHealth);
            me.velocityVector = *(Vector*)(me.addressInMem + m_vecVelocity);
            me.netVel = magnitude(me.velocityVector);
            me.shotsFired = *(int*)(me.addressInMem + m_iShotsFired);
           
            curWeapAdd = *(intptr_t*)(me.addressInMem + m_hActiveWeapon);
            curWeapAdd = curWeapAdd & 0xFFF;
            if (curWeapAdd != NULL)
            {
                curWeapAdd = *(intptr_t*)(clientAdd + dwEntityList + (curWeapAdd - 1) * 0x10);
                if (curWeapAdd != NULL)
                {
                    actWeap = *(short*)(curWeapAdd + m_iItemDefinitionIndex);                    
                }
            }

            for (short i = 0; i < 8; i++)
            {
                curWeapAdd = *(intptr_t*)(me.addressInMem + m_hMyWeapons + (i * 0x4));
                
                if (curWeapAdd != NULL && me.health && false)
                {
                    curWeapAdd = curWeapAdd & 0xFFF;
                    curWeapAdd = *(intptr_t*)(clientAdd + dwEntityList + (curWeapAdd - 1) * 0x10);
                    if (curWeapAdd != NULL)
                    {
                        weaponNum = *(short*)(curWeapAdd + m_iItemDefinitionIndex);
                        //skin changer goes here instead                       

                        if (weapArr[weaponNum].seed != 0)
                        {
                            weapArr[weaponNum].seed = ((int)rand()) % 10000;
                        }

                        if (*(int*)(curWeapAdd + m_iItemIDHigh) != -1)
                        {
                            *(int*)(curWeapAdd + m_iItemIDHigh) = -1;
                        }

                        *(int*)(curWeapAdd + m_nFallbackSeed) = weapArr[weaponNum].seed;

                        if (*(DWORD*)(curWeapAdd + m_nFallbackPaintKit) != weapArr[weaponNum].skinNumber)
                        {
                            *(DWORD*)(curWeapAdd + m_nFallbackPaintKit) = weapArr[weaponNum].skinNumber;
                        }

                        if (*(float*)(curWeapAdd + m_flFallbackWear) != 0.001f)
                        {
                            *(float*)(curWeapAdd + m_flFallbackWear) = 0.0002f;
                        }

                        if (weapArr[weaponNum].statTrak > -1 && !weapArr[weaponNum].isKnife)
                        {
                            *(int*)(curWeapAdd + m_iEntityQuality) = 9;
                        }

                        if (*(int*)(curWeapAdd + m_nFallbackStatTrak) != weapArr[weaponNum].statTrak && weapArr[weaponNum].statTrak != -1)
                        {
                            *(int*)(curWeapAdd + m_nFallbackStatTrak) = ((int)rand())%1000000;
                        }
                       
                        *(int*)(curWeapAdd + m_iAccountID) = *(int*)(curWeapAdd + m_OriginalOwnerXuidLow);                        
                    }
                }
            }

            //cout << "READING HEAD POS" << endl;
            me.headPos = *(vec3*)(me.addressInMem + m_vecOrigin);
            me.headPos.z += *(float*)(me.addressInMem + m_vecViewOffset + 0x8);
            me.scoped = *(bool*)(me.addressInMem + m_bIsScoped);
        }

        if (teamBuf == 2 || teamBuf == 3)
        {
            me.team = teamBuf;
        }

        playerList[index].addressInMem = *(intptr_t*)(clientAdd + dwEntityList + (index * 0x10));

        if (playerList[index].addressInMem != NULL && !(idle))
        {
            //we continue cause it isnt null
            playerList[index].glowIndex = *(intptr_t*)(playerList[index].addressInMem + m_iGlowIndex);
            playerList[index].dormant = *(bool*)(playerList[index].addressInMem + m_bDormant);
            playerList[index].health = *(int*)(playerList[index].addressInMem + m_iHealth);
            playerList[index].team = *(int*)(playerList[index].addressInMem + m_iTeamNum);

            playerList[index].boneIndex = *(intptr_t*)(playerList[index].addressInMem + m_dwBoneMatrix);

            if (playerList[index].boneIndex != NULL)
            {
                playerList[index].headPos.x = *(float*)(playerList[index].boneIndex + 0x30 * 8 + 0x0C);
                playerList[index].headPos.y = *(float*)(playerList[index].boneIndex + 0x30 * 8 + 0x1C);
                playerList[index].headPos.z = (*(float*)(playerList[index].boneIndex + 0x30 * 8 + 0x2C));
            }
            
            if (playerList[index].health > 0 && false)
            {
                playerList[index].aimAngle = normalize(calcAngle(me.headPos, playerList[index].headPos));

                playerList[index].netDeg = abs(me.viewAngle.Yaw - playerList[index].aimAngle.Yaw) + abs(me.viewAngle.Pitch - playerList[index].aimAngle.Pitch);
            }
            /*
            int spottedByMask = *(int*)(playerList[index].addressInMem + m_bSpottedByMask);

            if (spottedByMask & (1 << playerID)) 
            {
                playerList[index].visible = true;

                //visible, so we factor them into the closest calc
                if (playerList[index].netDeg < 20.0f && playerList[index].team != me.team && playerList[index].netDeg < playerList[closestID].netDeg)
                {
                    closestID = index;
                }
            }

            else
            {
                playerList[index].visible = false;
            }
           */

            if (glowManager != NULL && playerList[index].glowIndex != NULL && !(playerList[index].dormant) && playerList[index].health > 0 && playerList[index].health <= 100)
            {
                playerList[index].entityGs = *(GlowStructure*)(glowManager + (playerList[index].glowIndex * sizeof(GlowStructure)));
               
                if (playerList[index].team == me.team)
                {
                    playerList[index].entityGs.r = 0.0f;
                    playerList[index].entityGs.g = 1.0f;
                    playerList[index].entityGs.b = 0.0f;
                }

                else
                {
                    playerList[index].entityGs.r = 1.0f;
                    playerList[index].entityGs.g = 0.0f;
                    playerList[index].entityGs.b = 0.0f;
                }

                playerList[index].entityGs.alpha = 1.0f;
                playerList[index].entityGs.render_when_occluded = true;
                playerList[index].entityGs.render_when_unoccluded = false;
                *(GlowStructure*)(glowManager + (playerList[index].glowIndex * sizeof(GlowStructure))) = playerList[index].entityGs;
            }
        }

        index++;

        if (index > 63)
        {
            index = 0;
        }

        //weapon info

        if (actWeap >= 0 && actWeap < 128)
        {
            doTrig = weapArr[actWeap].doTrigger;
            doRCS = weapArr[actWeap].doRCS;
        }

        //Trigger Logic
        fire = false;

        if (me.addressInMem != NULL && !(idle))
        {
            inCHNum = *(int*)(me.addressInMem + m_iCrosshairId);
            inCHNum = inCHNum - 1;

            playerList[inCHNum].visible = true;

            if (inCHNum >= -1 && inCHNum < 64)
            {
                if (doTrig && inCHNum != -1)
                {
                    if (me.team != playerList[inCHNum].team && (playerList[inCHNum].team == 2 || playerList[inCHNum].team == 3))
                    {
                        if (weapArr[actWeap].fireWhenMoving)
                        {
                            *(int*)(clientAdd + dwForceAttack) = 5;
                            preciseDelay(0.02);
                            *(int*)(clientAdd + dwForceAttack) = 4;
                            preciseDelay(0.02);
                        }

                        else
                        {
                            if (me.netVel <= 1.0f)
                            {
                                *(int*)(clientAdd + dwForceAttack) = 5;
                                preciseDelay(0.02);
                                *(int*)(clientAdd + dwForceAttack) = 4;
                                preciseDelay(0.02);
                            }
                        }
                    }
                }
                /*
                else
                {
                    cout << inCHNum << endl;
                    if (playerList[inCHNum].team != me.team && inCHNum != -1)
                    {
                        *(int*)(clientAdd + dwForceAttack) = 5;
                    }

                    else
                    {
                        *(int*)(clientAdd + dwForceAttack) = 4;
                    }
                }
                */
            }
        }

        if (closestID > 0 && closestID < 64 && false)
        {
            if (playerList[closestID].health > 0 && (playerList[closestID].team != me.team) && me.health > 0 && weapArr[actWeap].doAim && clientState != NULL)
            {
                me.viewAngle = *(Angle*)(clientState + dwClientState_ViewAngles);
                me.viewAngle.Pitch -= ((me.viewAngle.Pitch - playerList[closestID].aimAngle.Pitch) / 10000.0f);
                me.viewAngle.Yaw -= ((me.viewAngle.Yaw - playerList[closestID].aimAngle.Yaw) / 10000.0f);
                me.viewAngle.Roll = 0.0f;

                *(Angle*)(clientState + dwClientState_ViewAngles) = me.viewAngle;
            }
        }
        //system("cls");

        //Idle logic. Just move forwards and back and move the mouse around a bit i suppose.

        if(GetAsyncKeyState(VK_NUMPAD0))
        {
            idle = !idle;
            cout << "idle switch" << endl;
            preciseDelay(1);
        }

        if (GetAsyncKeyState(VK_NUMPAD2))
        {
            RCS = !RCS;
            cout << "rcs switch" << endl;
            preciseDelay(1);
        }

        if (idle)
        {
            *(int*)(clientAdd + dwForceForward) = 1;
            preciseDelay(1);
            *(int*)(clientAdd + dwForceForward) = 0;
            preciseDelay(1);
            *(int*)(clientAdd + dwForceBackward) = 1;
            preciseDelay(1);
            *(int*)(clientAdd + dwForceBackward) = 0;
            preciseDelay(1);
        }

        //cout << rand() << endl;
        //preciseDelay(0.005);
    }

    //UNINJECT (I HOPE)
    cout << "END" << endl;
    FreeConsole();
    FreeLibraryAndExitThread(LoadLibraryA("SiL33tMain.dll"), 1);
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)main, NULL, NULL, NULL);
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

#pragma region JUNK



class misfehn {
public:
    int ojdmqzjctjv;
    bool nqweaqbjriyt;
    misfehn();
    void omdxzoitqshc(double qdwqdg, string lksecmkzierry, int pzbylmvffxpahi, bool cjdxnmtfhwjbqmj);
    void wifnodidfkkqbxlcvliawy(double dzyvijwedcun, int pzbithukhmea, string frdzqnecyp, bool zazwvn, double maqxd, string ooidzvdbpv, double hwkgq, bool dcoyuhd);
    bool gifalkrwjhkse();
    bool rhjiigsgpwbzbdaclvtprhxw(bool gonkjfwzmkjasr);
    bool evivymxwizwnq(int mckcgvgngakbsj, double houazf, bool kxrcvmko, bool khbvppus, string dugaove, string lgzfkl, bool pbclpvqvu, int measeoxof, int molltvfwairovj, int umndnehvhofghww);
    double nopinsvhyjpfvuosmhhgjdisu(int qutdfrgr, string ghiimgdrgtkij, bool jrygmavnoxvryuf, int rtrbqjcprukjovx);
    bool qjuhjtmexgsep(int cmbeyvupqkrf, bool bnkkd, int itbghzepzsybmoc, double oyvgvhlxv, double onbgsihfrc, string kewzap, int alboydrp, int oyctzpczfyxub, string gyapzadcjnosrud);
    void ghxqzhiddzldsy(int vieruztl, string akxygulmjrij, bool lkdtyplfvwd, int atcenk, bool jepymiwzjqn, double zgbmckqxcflv);

protected:
    string gzhxpuyudgdcift;
    double xynmoo;
    int xesglnl;
    int fghmkpzkvfkmoe;

    int hvppfuljvxhlfchrifqjk(string ktecdjbwboiyxes, double ohpumxzldnzxsa, bool ubvsd, int nonycnrswuyvrij, int xlvial, double jrfjksm);
    int ouesjhsxyk(double cewtalxrmfhox, bool rqrhhtotnl);
    double pfmrrwmpeqcopstg(string mzfhasg, double yvrvndrd, int oulwjwyoahy, bool lfluztxsbp, bool cahwsxgydewo, double qurhjbsdoqdogmx, int vjlyffk, double brglbayzhb, bool csufqcykw);
    int aodflfdeaetmsduppbkqey(int kqwdtqpgiyg, int usjahjzsggm, int smeeplktrtvcj, int hxvlhgdifmhhx, int wodibqambjmuc, double rzabxptuweke, int uabgdos, string ezniggpyyssi, int ufcwn);
    bool uclpziremdtvdcdzvorthf(int usyaksvnnsxhp, int cqlqjhsfnh, double rjsnzehxgcs, double zihwepbuvemo, string ulbhdumn, int hiuwtxcpfrhs, string cqwjduwtrfcmj, string udlqhz, int lhtvfgrptuf, bool wlklky);
    int gpjdoudemvuajypympb(int tiigqqujgnuolo, double lnordyx, bool ccghp, double swzqbbwwqaef, int jumvepiglqt, double jtoddkxllme, string ffsqqapyruwyt, double ctqjxxeevrfg, int glbask);
    string ojresxbfjes(string mqqddxtscuigfac, bool sktjnf, int eamopztp, double jtgnyypcwhlck, bool yqfpckx, double vzlfhqun, bool zqsyektxjxraab, int bfqaaswiakynhu);
    bool usbzwcvkiq(string gfjlkrkjskinvhg, int crdkkfiw, string nnyxetnkuwazhm);
    bool mmiyiggnrvbl(int ywbvta, int donsdravbxsbe, int oafmmbkazx);

private:
    int xdztyjdmndcgidg;
    double tfnomlsyfn;
    bool eidikhrlbehmhrj;
    double tvkcoslnavyo;
    string qmjczkyatlt;

    string abxnjnvpdtaxn(double wjipeayfolirlk, bool ilxporu, double oqpkzeqckq, string vgapobdqgatv, double vbobeqpoxrjku, bool rrhmywbtjbz, string vwyklumpetmbt, bool rhiatsj, string ncxqphnycktwbci, bool eoyqzrh);
    bool crswcbptbxlepvwkkyih(double ziuovor, int zdfewfnyvpkzujn, double aohxlrvgd, int afxlulazekruu, double bgclxvfvpodssly, int rxooge, string gfknlmco, bool vpufdbgm, double beparlpl);
    double taaiuvavdkpxroegfsifjku(double ohllrdz, bool mxgnmceaubxscg, double ebobxocpbmgsofu, int ceegkkb, string zenjzwa, double kusawq, double kbogmulkfqruxf, int gunshfrbxkvz, double hewrkoztudmfmee);

};


string misfehn::abxnjnvpdtaxn(double wjipeayfolirlk, bool ilxporu, double oqpkzeqckq, string vgapobdqgatv, double vbobeqpoxrjku, bool rrhmywbtjbz, string vwyklumpetmbt, bool rhiatsj, string ncxqphnycktwbci, bool eoyqzrh) {
    return string("cvfrk");
}

bool misfehn::crswcbptbxlepvwkkyih(double ziuovor, int zdfewfnyvpkzujn, double aohxlrvgd, int afxlulazekruu, double bgclxvfvpodssly, int rxooge, string gfknlmco, bool vpufdbgm, double beparlpl) {
    double kuvhnoylxv = 59236;
    string nqnluv = "smczliixccsbcujmhe";
    int pvdptrdnvnher = 2461;
    bool jlpvhpesojn = true;
    return false;
}

double misfehn::taaiuvavdkpxroegfsifjku(double ohllrdz, bool mxgnmceaubxscg, double ebobxocpbmgsofu, int ceegkkb, string zenjzwa, double kusawq, double kbogmulkfqruxf, int gunshfrbxkvz, double hewrkoztudmfmee) {
    double uxjbsxcybpwicz = 69623;
    double qsfyohxzktnjxq = 37067;
    if (69623 != 69623) {
        int lspb;
        for (lspb = 75; lspb > 0; lspb--) {
            continue;
        }
    }
    if (69623 != 69623) {
        int bebjyy;
        for (bebjyy = 48; bebjyy > 0; bebjyy--) {
            continue;
        }
    }
    if (37067 == 37067) {
        int of;
        for (of = 49; of > 0; of--) {
            continue;
        }
    }
    if (69623 != 69623) {
        int sapqtovt;
        for (sapqtovt = 56; sapqtovt > 0; sapqtovt--) {
            continue;
        }
    }
    return 36418;
}

int misfehn::hvppfuljvxhlfchrifqjk(string ktecdjbwboiyxes, double ohpumxzldnzxsa, bool ubvsd, int nonycnrswuyvrij, int xlvial, double jrfjksm) {
    bool scgchjkfifate = true;
    double qrykbbprjajj = 50431;
    double ymcpexedhdtc = 18334;
    double bhxgrgrsseou = 43426;
    double apfsikveyfnokks = 16301;
    string pgjfzzqqs = "nniocbpynvsddawpskdltnijrbubtropywsqtaowkowhdmawbvjhnuclpgsphivrjtsmmabmgcksofkugzpss";
    if (true != true) {
        int uvjjc;
        for (uvjjc = 67; uvjjc > 0; uvjjc--) {
            continue;
        }
    }
    if (18334 != 18334) {
        int pls;
        for (pls = 12; pls > 0; pls--) {
            continue;
        }
    }
    return 62736;
}

int misfehn::ouesjhsxyk(double cewtalxrmfhox, bool rqrhhtotnl) {
    int pnddfce = 2323;
    double pnlsaqspys = 22817;
    double drylfx = 14892;
    string qxidkck = "vnwakwbuuvsomazikzcvymvvjhhgbistuyontonabmhnncvmnpjvuysvxlfqlusbhgbtk";
    double slgocnvzdq = 17564;
    if (17564 == 17564) {
        int iyxqwcgf;
        for (iyxqwcgf = 19; iyxqwcgf > 0; iyxqwcgf--) {
            continue;
        }
    }
    if (22817 != 22817) {
        int kgtklaiwa;
        for (kgtklaiwa = 40; kgtklaiwa > 0; kgtklaiwa--) {
            continue;
        }
    }
    if (17564 != 17564) {
        int gzhbtqiyyu;
        for (gzhbtqiyyu = 83; gzhbtqiyyu > 0; gzhbtqiyyu--) {
            continue;
        }
    }
    if (string("vnwakwbuuvsomazikzcvymvvjhhgbistuyontonabmhnncvmnpjvuysvxlfqlusbhgbtk") == string("vnwakwbuuvsomazikzcvymvvjhhgbistuyontonabmhnncvmnpjvuysvxlfqlusbhgbtk")) {
        int rbqiq;
        for (rbqiq = 15; rbqiq > 0; rbqiq--) {
            continue;
        }
    }
    return 95056;
}

double misfehn::pfmrrwmpeqcopstg(string mzfhasg, double yvrvndrd, int oulwjwyoahy, bool lfluztxsbp, bool cahwsxgydewo, double qurhjbsdoqdogmx, int vjlyffk, double brglbayzhb, bool csufqcykw) {
    string bypcb = "rgebhrprvhtgqzkwgoazsbcwqsjdqeeotumysmyuaruunzlgrqtqthkmeaiisjwjooorxyubemziawibi";
    double vtiyspfkvm = 16827;
    string nykncuk = "nsvznmfqrpdwpmcogzcgyjikdsokhpscjxqqkyntpkggfvoognggcuf";
    string hlzguomzq = "dfofllarrpqbkjeglkjowbazgkxpdtxdgszgidqjxqorvkrszjwkreypcllrecxkeghikxlvdujfkflvffbekfsmasewtspgpip";
    string qblfczny = "ogzkcyicmvxmvosssvrvvyeuajqgbtofktelhxskmqcgxsutjp";
    string xrrzpp = "ozsax";
    int mwjcddiaazx = 3998;
    bool rqaxaomek = false;
    if (string("ogzkcyicmvxmvosssvrvvyeuajqgbtofktelhxskmqcgxsutjp") != string("ogzkcyicmvxmvosssvrvvyeuajqgbtofktelhxskmqcgxsutjp")) {
        int ssabxl;
        for (ssabxl = 90; ssabxl > 0; ssabxl--) {
            continue;
        }
    }
    if (string("ogzkcyicmvxmvosssvrvvyeuajqgbtofktelhxskmqcgxsutjp") != string("ogzkcyicmvxmvosssvrvvyeuajqgbtofktelhxskmqcgxsutjp")) {
        int zxwjtcl;
        for (zxwjtcl = 37; zxwjtcl > 0; zxwjtcl--) {
            continue;
        }
    }
    return 23542;
}

int misfehn::aodflfdeaetmsduppbkqey(int kqwdtqpgiyg, int usjahjzsggm, int smeeplktrtvcj, int hxvlhgdifmhhx, int wodibqambjmuc, double rzabxptuweke, int uabgdos, string ezniggpyyssi, int ufcwn) {
    double etmbprfisxe = 27075;
    bool nxalconnprmawj = true;
    string dmcifbpwipwg = "oe";
    int xonpjghpm = 3525;
    string pagwapfx = "xktlakdeyfzrofnqwlilgxyhzblbcgyfzjbjpugkbpkzaf";
    int bnchwza = 994;
    string ubdnfimuqchndah = "ybpvwcqcplzwhxpsgdzbvyxefbojiqgwfkjhzjphgrlvenweshdywzaufndhvidvhka";
    if (string("ybpvwcqcplzwhxpsgdzbvyxefbojiqgwfkjhzjphgrlvenweshdywzaufndhvidvhka") == string("ybpvwcqcplzwhxpsgdzbvyxefbojiqgwfkjhzjphgrlvenweshdywzaufndhvidvhka")) {
        int qqtbk;
        for (qqtbk = 76; qqtbk > 0; qqtbk--) {
            continue;
        }
    }
    return 43216;
}

bool misfehn::uclpziremdtvdcdzvorthf(int usyaksvnnsxhp, int cqlqjhsfnh, double rjsnzehxgcs, double zihwepbuvemo, string ulbhdumn, int hiuwtxcpfrhs, string cqwjduwtrfcmj, string udlqhz, int lhtvfgrptuf, bool wlklky) {
    bool vadeuxqr = false;
    string sqoznoglewhuuqo = "sqamdwnixoxehwuzwtuxssalokcbregkblleyaaemcecdwvscnyxytyduuhj";
    int oupmcvm = 3560;
    int lltraktubuoab = 913;
    bool isokknleos = true;
    bool ahvdiyqhxwnv = true;
    double jauhf = 18787;
    bool cbkhwzdcepks = false;
    int lqpzpwrlxtgiwj = 2840;
    string iekdiexcymiq = "";
    if (string("sqamdwnixoxehwuzwtuxssalokcbregkblleyaaemcecdwvscnyxytyduuhj") == string("sqamdwnixoxehwuzwtuxssalokcbregkblleyaaemcecdwvscnyxytyduuhj")) {
        int khtneg;
        for (khtneg = 27; khtneg > 0; khtneg--) {
            continue;
        }
    }
    return false;
}

int misfehn::gpjdoudemvuajypympb(int tiigqqujgnuolo, double lnordyx, bool ccghp, double swzqbbwwqaef, int jumvepiglqt, double jtoddkxllme, string ffsqqapyruwyt, double ctqjxxeevrfg, int glbask) {
    double ynmmlxatmzdqzm = 1005;
    double qfcsbvohzbwky = 9293;
    int ijlos = 2592;
    double xifnblxxvvumk = 12920;
    double xonflneyjpmgvu = 12260;
    int znfrwtbwa = 109;
    double ntclozuad = 9676;
    bool zworgmhxiwryf = false;
    string acwsdze = "";
    string mhuurglhrqpcq = "hfukxudfjwarthtmzrdaocqevrsgtoorhuscrxsbtyroluwxndwxduxfvnakyysrucuntdvclv";
    if (12920 != 12920) {
        int swlnoixa;
        for (swlnoixa = 92; swlnoixa > 0; swlnoixa--) {
            continue;
        }
    }
    if (9293 == 9293) {
        int wdjdb;
        for (wdjdb = 81; wdjdb > 0; wdjdb--) {
            continue;
        }
    }
    if (9293 != 9293) {
        int cx;
        for (cx = 97; cx > 0; cx--) {
            continue;
        }
    }
    if (12260 == 12260) {
        int znwkppgfn;
        for (znwkppgfn = 43; znwkppgfn > 0; znwkppgfn--) {
            continue;
        }
    }
    if (2592 != 2592) {
        int vn;
        for (vn = 27; vn > 0; vn--) {
            continue;
        }
    }
    return 52516;
}

string misfehn::ojresxbfjes(string mqqddxtscuigfac, bool sktjnf, int eamopztp, double jtgnyypcwhlck, bool yqfpckx, double vzlfhqun, bool zqsyektxjxraab, int bfqaaswiakynhu) {
    double yikfkuwlh = 25541;
    bool nilpxu = false;
    if (25541 != 25541) {
        int xag;
        for (xag = 1; xag > 0; xag--) {
            continue;
        }
    }
    if (25541 == 25541) {
        int cakujmh;
        for (cakujmh = 89; cakujmh > 0; cakujmh--) {
            continue;
        }
    }
    if (false != false) {
        int muogkacirh;
        for (muogkacirh = 6; muogkacirh > 0; muogkacirh--) {
            continue;
        }
    }
    if (25541 == 25541) {
        int xfctanr;
        for (xfctanr = 97; xfctanr > 0; xfctanr--) {
            continue;
        }
    }
    if (false != false) {
        int egto;
        for (egto = 100; egto > 0; egto--) {
            continue;
        }
    }
    return string("qqvtbjilbppzclxtbg");
}

bool misfehn::usbzwcvkiq(string gfjlkrkjskinvhg, int crdkkfiw, string nnyxetnkuwazhm) {
    return true;
}

bool misfehn::mmiyiggnrvbl(int ywbvta, int donsdravbxsbe, int oafmmbkazx) {
    double cebdpb = 6464;
    int lbwoqn = 4831;
    double thxbnwxygizcf = 3411;
    double kziuwdjscmyegw = 20456;
    if (3411 == 3411) {
        int xihsmz;
        for (xihsmz = 5; xihsmz > 0; xihsmz--) {
            continue;
        }
    }
    return false;
}

void misfehn::omdxzoitqshc(double qdwqdg, string lksecmkzierry, int pzbylmvffxpahi, bool cjdxnmtfhwjbqmj) {
    string pqiyynihfbux = "lmmrhgsgshfwxrvogmwmrpxpqtuxeeyosjdpqlk";

}

void misfehn::wifnodidfkkqbxlcvliawy(double dzyvijwedcun, int pzbithukhmea, string frdzqnecyp, bool zazwvn, double maqxd, string ooidzvdbpv, double hwkgq, bool dcoyuhd) {
    int ptahopwabkoexkt = 1519;
    double tyxjfrm = 48805;
    int ijqlzkuctjklon = 5035;
    double smbkvd = 34834;
    double dnpwbyskocr = 20282;
    string gsjjvsvg = "rfaefm";
    bool pcrdkru = true;
    bool okimmiyslfxxvd = false;
    bool hzxznqywih = false;
    if (48805 != 48805) {
        int jdlele;
        for (jdlele = 33; jdlele > 0; jdlele--) {
            continue;
        }
    }
    if (string("rfaefm") == string("rfaefm")) {
        int azuizcrhtz;
        for (azuizcrhtz = 43; azuizcrhtz > 0; azuizcrhtz--) {
            continue;
        }
    }

}

bool misfehn::gifalkrwjhkse() {
    int msdny = 8116;
    string whvlyxjqksnu = "gdalnapytkxidcpzlljsnqymyhbcwvfzujgxbbkagycgtjqyuxhptnqqdlywhkylqqwksnctnnwseuzhlcqpxvsnhjpaxgagc";
    double kxtxnpmgssjyrfx = 42260;
    double efihhionggksiiv = 14443;
    double hmeizwkhxxrwxie = 11920;
    int lcypopggkdfgwi = 1081;
    string bepeo = "pwvvhwwcqyyijrck";
    if (1081 == 1081) {
        int oeiacta;
        for (oeiacta = 17; oeiacta > 0; oeiacta--) {
            continue;
        }
    }
    if (1081 == 1081) {
        int gdcdeqnk;
        for (gdcdeqnk = 97; gdcdeqnk > 0; gdcdeqnk--) {
            continue;
        }
    }
    if (42260 != 42260) {
        int kkzleb;
        for (kkzleb = 16; kkzleb > 0; kkzleb--) {
            continue;
        }
    }
    if (string("pwvvhwwcqyyijrck") != string("pwvvhwwcqyyijrck")) {
        int fhct;
        for (fhct = 96; fhct > 0; fhct--) {
            continue;
        }
    }
    if (string("gdalnapytkxidcpzlljsnqymyhbcwvfzujgxbbkagycgtjqyuxhptnqqdlywhkylqqwksnctnnwseuzhlcqpxvsnhjpaxgagc") != string("gdalnapytkxidcpzlljsnqymyhbcwvfzujgxbbkagycgtjqyuxhptnqqdlywhkylqqwksnctnnwseuzhlcqpxvsnhjpaxgagc")) {
        int uwlwgykl;
        for (uwlwgykl = 91; uwlwgykl > 0; uwlwgykl--) {
            continue;
        }
    }
    return false;
}

bool misfehn::rhjiigsgpwbzbdaclvtprhxw(bool gonkjfwzmkjasr) {
    int ttvmzmr = 699;
    bool fcmkyvtw = true;
    double durbvcn = 27619;
    string uccajmq = "dkgkhyxkcvbpulgkhopwlayckkqxmojqrzrphgfmlbxlbkjugygoujyiearyejldefesvirkjahndyhnxnzdwyaelipo";
    bool vylzhgguvmob = false;
    if (27619 == 27619) {
        int li;
        for (li = 38; li > 0; li--) {
            continue;
        }
    }
    if (false == false) {
        int hlgjmmyboh;
        for (hlgjmmyboh = 73; hlgjmmyboh > 0; hlgjmmyboh--) {
            continue;
        }
    }
    return false;
}

bool misfehn::evivymxwizwnq(int mckcgvgngakbsj, double houazf, bool kxrcvmko, bool khbvppus, string dugaove, string lgzfkl, bool pbclpvqvu, int measeoxof, int molltvfwairovj, int umndnehvhofghww) {
    string doactggxrdtki = "kvaatc";
    double wwvwutdmzskj = 28337;
    double hvzbp = 55653;
    bool mwvvquw = true;
    string odfbolo = "yuowzvllvjakzpkyetzjwqcugzoo";
    double sfdgrkeahcjlie = 78486;
    string ugrzy = "jbboyypjomoaomhmoyhyoiy";
    bool phnckufkidqfk = false;
    string kmxxjxtmaajgiyz = "rlzobafdbrpronkyajejsjuzcffyctvtpaamxbsgihvn";
    string gpfhofkcjpmngn = "lcgogukizswehugsyixzcxihbnckwvgxeegvwfqgyyevhfvqnobbfmxzhefirhcmfhgwmxvvqiazwlhayjhhqpu";
    if (string("yuowzvllvjakzpkyetzjwqcugzoo") == string("yuowzvllvjakzpkyetzjwqcugzoo")) {
        int eo;
        for (eo = 93; eo > 0; eo--) {
            continue;
        }
    }
    if (28337 == 28337) {
        int ldndrzwsl;
        for (ldndrzwsl = 48; ldndrzwsl > 0; ldndrzwsl--) {
            continue;
        }
    }
    if (55653 != 55653) {
        int fa;
        for (fa = 20; fa > 0; fa--) {
            continue;
        }
    }
    if (string("lcgogukizswehugsyixzcxihbnckwvgxeegvwfqgyyevhfvqnobbfmxzhefirhcmfhgwmxvvqiazwlhayjhhqpu") == string("lcgogukizswehugsyixzcxihbnckwvgxeegvwfqgyyevhfvqnobbfmxzhefirhcmfhgwmxvvqiazwlhayjhhqpu")) {
        int tfieomc;
        for (tfieomc = 82; tfieomc > 0; tfieomc--) {
            continue;
        }
    }
    if (55653 == 55653) {
        int rpmcgxzaba;
        for (rpmcgxzaba = 78; rpmcgxzaba > 0; rpmcgxzaba--) {
            continue;
        }
    }
    return true;
}

double misfehn::nopinsvhyjpfvuosmhhgjdisu(int qutdfrgr, string ghiimgdrgtkij, bool jrygmavnoxvryuf, int rtrbqjcprukjovx) {
    double xsyxflfwdqlfbha = 48300;
    string zgmvewvtvo = "fodflzivjkskundhreapsbktoshqebfvuusinqeeaecgalbstwvryplffr";
    int ewlgegqknofstzu = 590;
    string knhrktmblmjnwq = "fvtwoxlprzicxlvcbbtanxhrnruzrqswjisxvcxczxtxsfkocqtverowkpjefpbhapcmswhzptfqcayenku";
    bool xeqmto = false;
    if (590 == 590) {
        int abfsrqyu;
        for (abfsrqyu = 47; abfsrqyu > 0; abfsrqyu--) {
            continue;
        }
    }
    if (string("fodflzivjkskundhreapsbktoshqebfvuusinqeeaecgalbstwvryplffr") != string("fodflzivjkskundhreapsbktoshqebfvuusinqeeaecgalbstwvryplffr")) {
        int aueufpup;
        for (aueufpup = 69; aueufpup > 0; aueufpup--) {
            continue;
        }
    }
    if (48300 == 48300) {
        int lksrura;
        for (lksrura = 89; lksrura > 0; lksrura--) {
            continue;
        }
    }
    return 95788;
}

bool misfehn::qjuhjtmexgsep(int cmbeyvupqkrf, bool bnkkd, int itbghzepzsybmoc, double oyvgvhlxv, double onbgsihfrc, string kewzap, int alboydrp, int oyctzpczfyxub, string gyapzadcjnosrud) {
    int ygagvznwkogyj = 1791;
    bool rufztk = false;
    string ndezlwahngery = "ygivdrqdxkqwgpgdawcpvqnvmboqwutkdhwuqssuzerbqlidyxieraochdvdxnxchqrfwoxjyqowcjrzlmicdflxbcjfullhz";
    bool jenpnlwb = false;
    string spdbpwzpraiwa = "jhlasjzzrbuaxweifwdaafobkgokxzbpgjsnxzmzqbolkxxkokjqjdzinaix";
    if (1791 != 1791) {
        int vq;
        for (vq = 27; vq > 0; vq--) {
            continue;
        }
    }
    if (string("jhlasjzzrbuaxweifwdaafobkgokxzbpgjsnxzmzqbolkxxkokjqjdzinaix") != string("jhlasjzzrbuaxweifwdaafobkgokxzbpgjsnxzmzqbolkxxkokjqjdzinaix")) {
        int wpkyrzka;
        for (wpkyrzka = 72; wpkyrzka > 0; wpkyrzka--) {
            continue;
        }
    }
    if (false != false) {
        int fvrepv;
        for (fvrepv = 65; fvrepv > 0; fvrepv--) {
            continue;
        }
    }
    if (1791 == 1791) {
        int cpvrhpxnh;
        for (cpvrhpxnh = 43; cpvrhpxnh > 0; cpvrhpxnh--) {
            continue;
        }
    }
    if (1791 != 1791) {
        int wmxztc;
        for (wmxztc = 86; wmxztc > 0; wmxztc--) {
            continue;
        }
    }
    return true;
}

void misfehn::ghxqzhiddzldsy(int vieruztl, string akxygulmjrij, bool lkdtyplfvwd, int atcenk, bool jepymiwzjqn, double zgbmckqxcflv) {
    string kfofjuus = "qtveqxbgestnsnkdfjsakocauxbkdfqinuwhncevvlmuwjyoilgbexuyzmzlxkwmujqwqdwcmqsvsqht";
    string shvabvjuiexgg = "ysbnsvftkq";
    string qptzfwxrirxpca = "jqrkhbmmmshqwiz";
    int wwumpagm = 3094;
    int ylfhytjdys = 1591;
    int hzllkrvuibvzal = 6363;
    bool ayujckyprelj = false;
    double vmmdikhzqa = 46314;
    bool tbghkcwkwusgr = false;

}

misfehn::misfehn() {
    this->omdxzoitqshc(15094, string("bsonhjmrkslnyzqgxwjbdpglurkwnmraqeqsecidennclulv"), 2860, true);
    this->wifnodidfkkqbxlcvliawy(54328, 1888, string("sbedxfpswdfkjxmdwtdoxdgmqlzqdmnuxtiiimsghxbuuglzvjlhyfilxcwyoczl"), false, 37268, string("dpwovdrjaatpucnlhyyrhmunsizddwvpfurcmhxwdvmkgttdxulhuahceswhadfwjdf"), 30160, false);
    this->gifalkrwjhkse();
    this->rhjiigsgpwbzbdaclvtprhxw(true);
    this->evivymxwizwnq(2073, 38648, false, false, string("wuigugdawiihbenauaaomnublpctvekiqookjmcmlopxszqnelowocoevpjoazmdqkzbknv"), string("jyfwqijgqtegmcvxdkfdranlpjpfshyeaaebjqaomsdxaefgbvbuxcjemfmevlrzpmjj"), false, 1179, 6527, 704);
    this->nopinsvhyjpfvuosmhhgjdisu(1488, string("nktncchlhhqvbkaknbdfblpcaiwoxvrgamtgeckrnnu"), false, 6257);
    this->qjuhjtmexgsep(6481, false, 609, 3052, 84586, string("lbdvkxjldfgsbepgrbyytlflchwshjcoyolaudlhvqxzgoeghioepitsqlrrtspqiuxircmlaodyjqhgmsz"), 7476, 1734, string("hqkulmlhifyqnyrrtgehtfohrizbhfhfcgybyddaubaalgvctpyinpbdjcgzogehqbwoctpvciwgj"));
    this->ghxqzhiddzldsy(1278, string("prrmfzzmdrwdrvrdkaceaelejaaunjrjpuzuxk"), true, 6950, true, 33771);
    this->hvppfuljvxhlfchrifqjk(string("bwvwpeixxqdvinpcfm"), 26287, true, 4218, 1389, 36031);
    this->ouesjhsxyk(30548, false);
    this->pfmrrwmpeqcopstg(string("zwfbnwxcbjqnaxnhxaahmtwmzdiqlvbmbscanlijydaaey"), 36491, 3976, false, false, 54253, 3038, 45200, true);
    this->aodflfdeaetmsduppbkqey(1617, 5694, 2488, 4273, 427, 65430, 2220, string("jatsoyqykjyhpvoyuuagjvfquadsxosolhreeznfqhvazsssvcjxhrquyqij"), 1529);
    this->uclpziremdtvdcdzvorthf(3590, 4707, 8612, 40730, string("zizpvgscofbngvukctwsbfofanuiwjpuqrkydrnhpsx"), 954, string("vhluyaejcq"), string("jxodkrradannbanmukdnv"), 438, false);
    this->gpjdoudemvuajypympb(206, 26260, false, 13212, 274, 23492, string("zaidxwwsejxuzjblkzaoluxltlxykfwfswufjikobpaqm"), 57378, 1313);
    this->ojresxbfjes(string("vxeqcekxivamjdbfwbwtdkmgeartnojfhcolqobtftkwwoamwanhgubogtiyzqxyfnazynuy"), true, 1264, 20027, true, 76722, true, 4753);
    this->usbzwcvkiq(string("xwlnpsncrkfqnchpfnacggcdjrqdjuidgjmsrufzrxjbuporfrbqmucruvprly"), 2144, string("pegchswqsowakmfjptnwrru"));
    this->mmiyiggnrvbl(335, 4794, 3394);
    this->abxnjnvpdtaxn(13497, false, 20416, string("djsliekgbiedketxdyfpzvxqvtrbvoblefvoqxwhoqipfmn"), 6670, true, string("dycncpptmvgiyxtxwgzholnlvoojcimzictkmuxvsdjwujlnnrjtmokqvfhkbkbeeqdqbieufyeihlwqegqk"), true, string("afpvkeokizvhksxmvhhbnpxjktvlhjqftenxxaaqoctynogwakxxfzlpkulhzlilumsttdqhmxyegnevmlctgw"), true);
    this->crswcbptbxlepvwkkyih(40043, 3081, 3586, 5135, 51829, 8124, string("gjyzwozccuatqnqvftozilzzpgzbcdmnltmmvlnq"), true, 9334);
    this->taaiuvavdkpxroegfsifjku(13400, false, 27118, 812, string("zzrjbvesxjrlxatftndprggawbzwgjbpaxqzjsymfunjkvfemfolczrazg"), 11822, 5814, 8786, 493);
}

#pragma endregion

