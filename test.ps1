param([ValidateSet('Debug', 'Release')][string]$Configuration = 'Debug')
$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot 'build.ps1') -Configuration $Configuration
& (Join-Path $PSScriptRoot "bin\$Configuration\CoreTests.exe")
if ($LASTEXITCODE) { throw "Core tests failed ($LASTEXITCODE)." }
& (Join-Path $PSScriptRoot "bin\$Configuration\CarrierTests.exe") $PSScriptRoot
if ($LASTEXITCODE) { throw "Carrier tests failed ($LASTEXITCODE)." }
