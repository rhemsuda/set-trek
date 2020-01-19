@ECHO OFF

del .\game_*.pdb

SET arg1=%1
IF "%arg1%"=="" (

    SET assimp_path=.\Include\assimp
    SET assimp_lib=.\Libraries\assimp-vc140-mt.lib

    SET dxtk_path=.\Include\DirectXTK\
    SET dxtk_lib=.\Libraries\DirectXTK.lib

    SET game_cpp=Source\Game.cpp Source\Texture.cpp Source\Planet.cpp Source\Ship.cpp Source\Vector.cpp Source\Sound.cpp Source\Rocket.cpp

    ECHO.
    ECHO Compiling and linking Game DLL...    
    cl /Zi /MD /EHsc /nologo /I%assimp_path% /I%dxtk_path% %game_cpp% /FeGame.dll /link -PDB:game_%random%.pdb /DLL -EXPORT:GameUpdateAndRender %assimp_lib% %dxtk_lib% User32.lib
    
    ECHO.
    ECHO Compiling and linking Main EXE...  
    cl /Zi /MD /EHsc /nologo /I%assimp_path% /I%dxtk_path% Source\main.cpp /link %dxtk_lib% User32.lib

) ELSE (

    ECHO.
    ECHO "Cleaning files"
    IF "%arg1%"=="clean" (
        del .\Main.exe
        del .\Game.dll
        del .\Game.lib
        del .\Gametemp.dll
        del .\*.obj
        del .\*.exp
        del .\*.pdb
        del .\*.ilk
    )	
)