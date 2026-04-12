# 脚本说明：使用Enigma Virtual Box将build/release目录下所有文件打包成一个单独的exe文件
# 请先下载并安装Enigma Virtual Box: https://enigmaprotector.com/en/downloads.html

# 设置路径
$baseDir = (Get-Item $PSScriptRoot).Parent.FullName
$releaseDir = Join-Path $baseDir "build\release"
$outputDir = Join-Path $baseDir "package\single_exe"
$evbPath = "C:\Program Files\Enigma Virtual Box\EVBConsole.exe"  # 请根据实际安装路径修改

# 创建输出目录
if (Test-Path $outputDir) {
    Remove-Item -Recurse -Force $outputDir
}
New-Item -ItemType Directory -Path $outputDir -Force

# 检查Enigma Virtual Box是否安装
if (-not (Test-Path $evbPath)) {
    Write-Host "错误: Enigma Virtual Box未安装或路径不正确。" -ForegroundColor Red
    Write-Host "请下载并安装Enigma Virtual Box: https://enigmaprotector.com/en/downloads.html" -ForegroundColor Yellow
    exit 1
}

# 检查build/release目录是否存在
if (-not (Test-Path $releaseDir)) {
    Write-Host "错误: build/release目录不存在，请先构建项目。" -ForegroundColor Red
    exit 1
}

# 检查ProductChecker.exe是否存在
$exePath = Join-Path $releaseDir "ProductChecker.exe"
if (-not (Test-Path $exePath)) {
    Write-Host "错误: ProductChecker.exe文件不存在，请先构建项目。" -ForegroundColor Red
    exit 1
}

# 创建EVB配置文件
$configFile = Join-Path $outputDir "config.txt"
$configContent = @"
; Enigma Virtual Box Configuration File
[Main]
File=ProductChecker.exe
OutputFile=$outputDir\ProductChecker_standalone.exe
Compress=1
SplashScreen=
SplashText=
SplashColor=
SplashTime=0
FileVersion=
ProductVersion=
CompanyName=
ProductName=
FileDescription=
Copyright=
InternalName=
OriginalFilename=

[Files]
; Add all files from release directory
"@

# 添加所有文件到配置
$files = Get-ChildItem -Path $releaseDir -Recurse -File
foreach ($file in $files) {
    $relativePath = $file.FullName.Replace($releaseDir, "").TrimStart("\")
    $configContent += "File=$($file.FullName)|$relativePath|0|0|0|0|0|0`n"
}

# 写入配置文件
$configContent | Out-File $configFile -Encoding ASCII

# 执行Enigma Virtual Box命令
Write-Host "正在使用Enigma Virtual Box打包文件..." -ForegroundColor Blue
& "$evbPath" "$configFile"

# 检查打包结果
$standaloneExe = Join-Path $outputDir "ProductChecker_standalone.exe"
if (Test-Path $standaloneExe) {
    Write-Host "打包成功！" -ForegroundColor Green
    Write-Host "独立可执行文件位于: $standaloneExe" -ForegroundColor Green
    Write-Host "现在您可以直接运行这个exe文件，它包含了所有必要的依赖库。" -ForegroundColor Green
} else {
    Write-Host "打包失败，请检查错误信息。" -ForegroundColor Red
}
