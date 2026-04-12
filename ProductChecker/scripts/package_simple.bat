@echo off

rem 设置基础目录
set "baseDir=%~dp0.."
set "packageDir=%baseDir%\package\release"
set "buildDir=%baseDir%\build\release"

echo Base directory: %baseDir%
echo Build directory: %buildDir%
echo Package directory: %packageDir%

rem 清理并创建打包目录
rd /s /q "%packageDir%" 2>nul
mkdir "%packageDir%"

rem 复制可执行文件
copy "%buildDir%\ProductChecker.exe" "%packageDir%"

rem 复制所有DLL文件
copy "%buildDir%\*.dll" "%packageDir%"

rem 复制所有插件目录
xcopy "%buildDir%\generic" "%packageDir%\generic" /s /e /y 2>nul
xcopy "%buildDir%\iconengines" "%packageDir%\iconengines" /s /e /y 2>nul
xcopy "%buildDir%\imageformats" "%packageDir%\imageformats" /s /e /y 2>nul
xcopy "%buildDir%\networkinformation" "%packageDir%\networkinformation" /s /e /y 2>nul
xcopy "%buildDir%\platforms" "%packageDir%\platforms" /s /e /y 2>nul
xcopy "%buildDir%\styles" "%packageDir%\styles" /s /e /y 2>nul
xcopy "%buildDir%\tls" "%packageDir%\tls" /s /e /y 2>nul
xcopy "%buildDir%\translations" "%packageDir%\translations" /s /e /y 2>nul

rem 创建启动脚本
echo @echo off > "%packageDir%\start.bat"
echo ProductChecker.exe >> "%packageDir%\start.bat"

echo 打包完成！
echo 打包文件位于: %packageDir%
echo 请运行 %packageDir%\start.bat 启动应用程序

pause
