@echo off
setlocal

rem 设置基础目录
set "baseDir=%~dp0.."
set "packageDir=%baseDir%\package\release"
set "buildDir=%baseDir%\build\release"

echo Base directory: %baseDir%
echo Build directory: %buildDir%
echo Package directory: %packageDir%

rem 创建打包目录
if exist "%packageDir%" (
    echo 清理现有打包目录...
    rd /s /q "%packageDir%"
)

mkdir "%packageDir%"
echo 创建打包目录成功: %packageDir%

rem 复制可执行文件
if exist "%buildDir%\ProductChecker.exe" (
    copy "%buildDir%\ProductChecker.exe" "%packageDir%"
    echo 复制可执行文件成功
) else (
    echo 错误: 未找到 ProductChecker.exe 文件，请确保已构建项目
    pause
    exit /b 1
)

rem 复制所有DLL文件
copy "%buildDir%\*.dll" "%packageDir%"
echo 复制DLL文件成功

rem 复制所有插件目录
set "pluginDirs=generic iconengines imageformats networkinformation platforms styles tls translations"
for %%d in (%pluginDirs%) do (
    if exist "%buildDir%\%%d" (
        mkdir "%packageDir%\%%d"
        xcopy "%buildDir%\%%d" "%packageDir%\%%d" /s /e /y
        echo 复制插件目录: %%d
    )
)

rem 创建启动脚本
echo @echo off > "%packageDir%\start.bat"
echo ProductChecker.exe >> "%packageDir%\start.bat"
echo 创建启动脚本成功

echo 打包完成！
echo 打包文件位于: %packageDir%
echo 请运行 %packageDir%\start.bat 启动应用程序

endlocal
pause
