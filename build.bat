@echo off

set BLibPath= "W:\BLib"
set JSONIncludePath="W:\json\include"

set commonCompilerFlags= -W1 -arch:AVX2 -wd4556 -wd4805 -nologo -Gm- -GR- -EHsc- -O2 -Oi -DPRINTINPUT=0 -DWIN32=1 -DDEBUG=1 -DPOOL_WIN32=1 -FC -Z7 -I %BLibPath% -I %JSONIncludePath%
set commonLinkerFlags= -PROFILE -NODEFAULTLIB:msvcrt.lib -incremental:no -opt:ref user32.lib shell32.lib gdi32.lib winmm.lib 
set renderExports=  -EXPORT:InitVulkan -EXPORT:BeginRender -EXPORT:DrawScreenSpaceGlyphEntities -EXPORT:DrawQuads -EXPORT:DrawScreenSpaceQuads -EXPORT:EndRender -EXPORT:DrawLineEntities -EXPORT:DrawPointEntities -EXPORT:CleanupVulkan -EXPORT:DrawStaticMeshEntities -EXPORT:EndRenderPass -EXPORT:BeginSecondaryRenderPass -EXPORT:DrawSDF -EXPORT:DrawTexturedQuadEntities -EXPORT:PrepareForRender -EXPORT:CopyMeshEntitiesToVertexBuffers
REM 32 bit build
REM cl %commonCompilerFlags% ..\asu\code\asu.cpp /link -subsystem:windows,5.1 %commonLinkerFlags%
REM cl %commonCompilerFlags% ..\asu\code\win32_asu.cpp /link -subsystem:windows,5.1 %commonLinkerFlags%

REM 64 bit build
REM /link
del *.pdb > NUL 2> NUL

rem win32

set name=struct_reflection
cl -MDd %commonCompilerFlags% -Fm%name%.map W:\%name%\%name%.cpp /link %commonLinkerFlags% /PDB:%name%_%random%.pdb

