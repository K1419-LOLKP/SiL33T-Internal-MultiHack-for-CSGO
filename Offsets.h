#pragma once
#include <cmath>

intptr_t cs_gamerules_data = 0x0;
intptr_t m_ArmorValue = 0xB378;
intptr_t m_Collision = 0x320;
intptr_t m_CollisionGroup = 0x474;
intptr_t m_Local = 0x2FBC;
intptr_t m_MoveType = 0x25C;
intptr_t m_OriginalOwnerXuidHigh = 0x31C4;
intptr_t m_OriginalOwnerXuidLow = 0x31C0;
intptr_t m_SurvivalGameRuleDecisionTypes = 0x1328;
intptr_t m_SurvivalRules = 0xD00;
intptr_t m_aimPunchAngle = 0x302C;
intptr_t m_aimPunchAngleVel = 0x3038;
intptr_t m_angEyeAnglesX = 0xB37C;
intptr_t m_angEyeAnglesY = 0xB380;
intptr_t m_bBombPlanted = 0x9A5;
intptr_t m_bFreezePeriod = 0x20;
intptr_t m_bGunGameImmunity = 0x3944;
intptr_t m_bHasDefuser = 0xB388;
intptr_t m_bHasHelmet = 0xB36C;
intptr_t m_bInReload = 0x32A5;
intptr_t m_bIsDefusing = 0x3930;
intptr_t m_bIsQueuedMatchmaking = 0x74;
intptr_t m_bIsScoped = 0x3928;
intptr_t m_bIsValveDS = 0x7C;
intptr_t m_bSpotted = 0x93D;
intptr_t m_bSpottedByMask = 0x980;
intptr_t m_bStartedArming = 0x33F0;
intptr_t m_bUseCustomAutoExposureMax = 0x9D9;
intptr_t m_bUseCustomAutoExposureMin = 0x9D8;
intptr_t m_bUseCustomBloomScale = 0x9DA;
intptr_t m_clrRender = 0x70;
intptr_t m_dwBoneMatrix = 0x26A8;
intptr_t m_fAccuracyPenalty = 0x3330;
intptr_t m_fFlags = 0x104;
intptr_t m_flC4Blow = 0x2990;
intptr_t m_flCustomAutoExposureMax = 0x9E0;
intptr_t m_flCustomAutoExposureMin = 0x9DC;
intptr_t m_flCustomBloomScale = 0x9E4;
intptr_t m_flDefuseCountDown = 0x29AC;
intptr_t m_flDefuseLength = 0x29A8;
intptr_t m_flFallbackWear = 0x31D0;
intptr_t m_flFlashDuration = 0xA420;
intptr_t m_flFlashMaxAlpha = 0xA41C;
intptr_t m_flLastBoneSetupTime = 0x2924;
intptr_t m_flLowerBodyYawTarget = 0x3A90;
intptr_t m_flNextAttack = 0x2D70;
intptr_t m_flNextPrimaryAttack = 0x3238;
intptr_t m_flSimulationTime = 0x268;
intptr_t m_flTimerLength = 0x2994;
intptr_t m_hActiveWeapon = 0x2EF8;
intptr_t m_hMyWeapons = 0x2DF8;
intptr_t m_hObserverTarget = 0x338C;
intptr_t m_hOwner = 0x29CC;
intptr_t m_hOwnerEntity = 0x14C;
intptr_t m_iAccountID = 0x2FC8;
intptr_t m_iClip1 = 0x3264;
intptr_t m_iCompetitiveRanking = 0x1A84;
intptr_t m_iCompetitiveWins = 0x1B88;
intptr_t m_iCrosshairId = 0xB3E4;
intptr_t m_iEntityQuality = 0x2FAC;
intptr_t m_iFOV = 0x31E4;
intptr_t m_iFOVStart = 0x31E8;
intptr_t m_iGlowIndex = 0xA438;
intptr_t m_iHealth = 0x100;
intptr_t m_iItemDefinitionIndex = 0x2FAA;
intptr_t m_iItemIDHigh = 0x2FC0;
intptr_t m_iMostRecentModelBoneCounter = 0x2690;
intptr_t m_iObserverMode = 0x3378;
intptr_t m_iShotsFired = 0xA390;
intptr_t m_iState = 0x3258;
intptr_t m_iTeamNum = 0xF4;
intptr_t m_lifeState = 0x25F;
intptr_t m_nFallbackPaintKit = 0x31C8;
intptr_t m_nFallbackSeed = 0x31CC;
intptr_t m_nFallbackStatTrak = 0x31D4;
intptr_t m_nForceBone = 0x268C;
intptr_t m_nTickBase = 0x3430;
intptr_t m_rgflCoordinateFrame = 0x444;
intptr_t m_szCustomName = 0x303C;
intptr_t m_szLastPlaceName = 0x35B4;
intptr_t m_thirdPersonViewAngles = 0x31D8;
intptr_t m_vecOrigin = 0x138;
intptr_t m_vecVelocity = 0x114;
intptr_t m_vecViewOffset = 0x108;
intptr_t m_viewPunchAngle = 0x3020;

intptr_t anim_overlays = 0x2980;
intptr_t clientstate_choked_commands = 0x4D30;
intptr_t clientstate_delta_ticks = 0x174;
intptr_t clientstate_last_outgoing_command = 0x4D2C;
intptr_t clientstate_net_channel = 0x9C;
intptr_t convar_name_hash_table = 0x2F0F8;
intptr_t dwClientState = 0x58DFE4;
intptr_t dwClientState_GetLocalPlayer = 0x180;
intptr_t dwClientState_IsHLTV = 0x4D48;
intptr_t dwClientState_Map = 0x28C;
intptr_t dwClientState_MapDirectory = 0x188;
intptr_t dwClientState_MaxPlayer = 0x388;
intptr_t dwClientState_PlayerInfo = 0x52C0;
intptr_t dwClientState_State = 0x108;
intptr_t dwClientState_ViewAngles = 0x4D90;
intptr_t dwEntityList = 0x4D9EAD4;
intptr_t dwForceAttack = 0x31D0034;
intptr_t dwForceAttack2 = 0x31D0040;
intptr_t dwForceBackward = 0x31D0088;
intptr_t dwForceForward = 0x31D0094;
intptr_t dwForceJump = 0x5248A34;
intptr_t dwForceLeft = 0x31D0010;
intptr_t dwForceRight = 0x31D0004;
intptr_t dwGameDir = 0x62C7F8;
intptr_t dwGameRulesProxy = 0x52BBD2C;
intptr_t dwGetAllClasses = 0xDAEC94;
intptr_t dwGlobalVars = 0x58DCE8;
intptr_t dwGlowObjectManager = 0x52E70D0;
intptr_t dwInput = 0x51F00B8;
intptr_t dwInterfaceLinkList = 0x943644;
intptr_t dwLocalPlayer = 0xD8722C;
intptr_t dwMouseEnable = 0xD8CDD0;
intptr_t dwMouseEnablePtr = 0xD8CDA0;
intptr_t dwPlayerResource = 0x31CE390;
intptr_t dwRadarBase = 0x51D384C;
intptr_t dwSensitivity = 0xD8CC6C;
intptr_t dwSensitivityPtr = 0xD8CC40;
intptr_t dwSetClanTag = 0x8A1A0;
intptr_t dwViewMatrix = 0x4D903D4;
intptr_t dwWeaponTable = 0x51F0B78;
intptr_t dwWeaponTableIndex = 0x325C;
intptr_t dwYawPtr = 0xD8CA30;
intptr_t dwZoomSensitivityRatioPtr = 0xD91CD0;
intptr_t dwbSendPackets = 0xD70FA;
intptr_t dwppDirect3DDevice9 = 0xA7050;
intptr_t find_hud_element = 0x3209ED20;
intptr_t force_update_spectator_glow = 0x3AE162;
intptr_t interface_engine_cvar = 0x3E9EC;
intptr_t is_c4_owner = 0x3BAC50;
intptr_t m_bDormant = 0xED;
intptr_t m_flSpawnTime = 0xA370;
intptr_t m_pStudioHdr = 0x294C;
intptr_t m_pitchClassPtr = 0x51D3AE8;
intptr_t m_yawClassPtr = 0xD8CA30;
intptr_t model_ambient_min = 0x59105C;
intptr_t set_abs_angles = 0x1E04E0;
intptr_t set_abs_origin = 0x1E0320;