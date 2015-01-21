// Header file which includes all Assimp header files and links against the Assimp library

#ifndef ASSIMP_INCLUDE_H
#define ASSIMP_INCLUDE_H

// assimp include files. These three are usually needed.
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/PostProcess.h"
#include "assimp/Scene.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"

// Link against the Assimp library.
#ifdef _DEBUG

#	pragma message("Linking against assimpD.lib")
#	pragma comment(lib, "assimpD.lib")

#	pragma message("Linking against zlibd.lib")
#	pragma comment(lib, "zlibd.lib")

#else

#	pragma message("Linking against assimp.lib")
#	pragma comment(lib, "assimp.lib")

#	pragma message("Linking against zlib.lib")
#	pragma comment(lib, "zlib.lib")

#endif	// _DEBUG

#endif	// ASSIMP_INCLUDE_H