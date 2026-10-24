param(
    [ValidateSet('Debug', 'Release')][string]$Configuration = 'Debug'
)
$ErrorActionPreference = 'Stop'
Push-Location $PSScriptRoot
try {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (!(Test-Path -LiteralPath $vswhere)) { throw 'Install Visual Studio with Desktop development with C++.' }
    $installation = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -format json | ConvertFrom-Json
    if (!$installation) { throw 'No Visual Studio C++ toolchain found.' }
    $msbuild = Join-Path $installation[0].installationPath 'MSBuild\Current\Bin\MSBuild.exe'
    $toolset = if ([version]$installation[0].installationVersion -ge [version]'18.0') { 'v145' } else { 'v143' }
    Push-Location build
    try {
        & .\premake5.exe vs2022
        if ($LASTEXITCODE) { throw "Premake failed ($LASTEXITCODE)." }
    } finally { Pop-Location }
    & $msbuild SecretFile.sln /m /nologo /v:minimal "/p:Configuration=$Configuration" /p:Platform=x64 "/p:PlatformToolset=$toolset"
    if ($LASTEXITCODE) { throw "Build failed ($LASTEXITCODE)." }
    $destination = Join-Path $PSScriptRoot "bin\$Configuration\resources"
    New-Item -ItemType Directory -Path $destination -Force | Out-Null
    Copy-Item -Path (Join-Path $PSScriptRoot 'resources\*') -Destination $destination -Recurse -Force
    Write-Output "Built bin\$Configuration\SecretFile.exe with runtime resources."
} finally { Pop-Location }
