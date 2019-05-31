chcp 65001
SETLOCAL ENABLEDELAYEDEXPANSION
echo off

::编译器路径
SET BinPath=..\..\Bin\
::工程名
SET ProjectName=test
::工程路径
SET DIR=%~dp0

::准备工作,进入工程文件夹,删除上次编译产生的文件
CD %DIR%
del *.hex
del 编译过程中产生的错误或警告.TXT

CLS
@echo [正在编译...]
for %%f in (.\*.c) do @echo "%%f" LARGE  WARNINGLEVEL (0) NOINTVECTOR NOPRINT>"%ProjectName%".__i && "%BinPath%C51.exe" @%ProjectName%.__i>>编译过程中产生的错误或警告.TXT
for %%f in (.\*.asm) do "%BinPath%A51.exe" "%%f">>编译过程中产生的错误或警告.TXT
if not exist *.obj goto End
@echo [正在连接...]
set objs=
for %%f in (.\*.obj) do (
set obj=%%f
if NOT "!objs!"=="" set objs=!objs!,"!obj!"
if "!objs!"=="" set objs="!obj!"
)
@echo %objs%>%ProjectName%.lnp
@echo TO "%ProjectName%">>%ProjectName%.lnp
@echo NOPRINT RAMSIZE(256) >>%ProjectName%.lnp
%BinPath%BL51 "@%ProjectName%.lnp">>编译过程中产生的错误或警告.TXT
@echo [正在生成HEX...]
%BinPath%OH51 "%ProjectName%">>编译过程中产生的错误或警告.TXT
if not exist %ProjectName%.hex goto End
::del 编译过程中产生的错误或警告.TXT
:end
@echo [正在删除垃圾文件...]
del %ProjectName%
del *.__i
del *.M51
del *.obj
del *.lnp
del *.lst
@echo [编译结束]
@type 编译过程中产生的错误或警告.txt
pause