@echo off
echo "<<< OpenSplice HDE Release V6.4.140407OSS For x86.win32, Date 2014-04-15 >>>"
if "%SPLICE_ORB%"=="" set SPLICE_ORB=DDS_OpenFusion_1_6_1
set BIOGEARS_ROOT=%~dp0\..\BioGears\BioGears_6.1.1-SDK-Windows
set OSPL_HOME=%~dp0\..\DDS\Implementations\OpenSplice\Windows
set PATH=%OSPL_HOME%\bin;%OSPL_HOME%\lib;%OSPL_HOME%\examples\lib;%PATH%
set OSPL_TMPL_PATH=%OSPL_HOME%\etc\idlpp
set OSPL_URI=file://%OSPL_HOME%\etc\config\ospl.xml
