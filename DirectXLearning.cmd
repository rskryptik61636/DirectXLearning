rem startup script for the DXLearning solution

rem setup the environment variables
rem NOTE: Windows DDK is a temporary workaround till we get our hands on VS2012, after which it can be removed
set DX_LEARNING_ROOT=N:\DirectXLearning\
set DXTK_ROOT=N:\DirectXTK\
set EFFECTS11_ROOT=N:\Effects11\
set NOTES_ROOT=N:\notes\
set RAPIDXML_ROOT=N:\rapidxml-1.13\
set ASSIMP_ROOT=N:\assimp--3.0.1270-sdk\
set VS_EXE="C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\IDE\devenv.exe"

rem start visual studio
start /D %DX_LEARNING_ROOT% /B %VS_EXE% %DX_LEARNING_ROOT%DirectXLearning.sln