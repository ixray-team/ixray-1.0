@echo off

cd libraries

coff2omf.exe BugTrap.lib BugTrapB.lib
coff2omf.exe ETools.lib EToolsB.lib
coff2omf.exe LWO.lib LWOB.lib
coff2omf.exe DXT.lib DXTB.lib
