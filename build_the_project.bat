@echo off
set /p projectName="Enter project name: "

:: Assemble the ASM file into object code
\masm32\bin\ml /c /Zd /coff %projectName%.asm

:: Link the object file to create an executable
\masm32\bin\Link /SUBSYSTEM:CONSOLE %projectName%.obj


pause
