#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

#ifndef _DEBUG
#define _DEBUG false
#else
#error "ERROR: DEBUG BUILDS NOT REQUIRED"
#endif

#ifndef XBOX_VER
#define XBOX_VER false
#endif

#if XBOX_VER
#pragma message("ATTENTION: BUILDING XBOX GAMEPASS VERSION")
#endif

#ifndef STEAM_DECK
#define STEAM_DECK true
#endif

#if STEAM_DECK
#pragma message("ATTENTION: BUILDING STEAM-DECK VERSION")
#endif

constexpr bool isDebug = _DEBUG;
constexpr bool isXbox = XBOX_VER;
constexpr bool isSteamDeck = STEAM_DECK;