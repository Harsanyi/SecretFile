$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot 'build.ps1') -Configuration Release
$stage = Join-Path $PSScriptRoot ('dist\stage-' + [guid]::NewGuid().ToString('N'))
$package = Join-Path $stage 'SecretFile'
New-Item -ItemType Directory -Path $package -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'bin\Release\SecretFile.exe') -Destination $package
Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'resources') -Destination $package -Recurse
Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'LICENSE'), (Join-Path $PSScriptRoot 'THIRD_PARTY_NOTICES.md') -Destination $package
Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'third_party') -Destination $package -Recurse
$instructions = @'
SecretFile - experimental low-order bit data embedding

Run SecretFile.exe with the resources directory beside it.
Drop a JPG/JPEG/PNG, WAV or OBJ carrier into the application.
Choose a nonzero BitDepth, press Open, then drop payload files into the right panel.
Save writes <carrier>_mod.png/.wav/.obj beside the input carrier.
Reopen that output at the same bit depth and use Exp to extract a file.
Extraction creates <carrier>_extracted beside the carrier and refuses existing targets.
BitDepth 0 is original preview only. Audio is normalized to 16-bit PCM; channels are preserved.
OBJ support is experimental and exports the first mesh only. External editing can destroy data.

This is not encryption and provides no cryptographic protection for sensitive data.
See LICENSE and THIRD_PARTY_NOTICES.md for licensing.
'@
[IO.File]::WriteAllText((Join-Path $package 'README.txt'), $instructions)
$zip = Join-Path $PSScriptRoot 'dist\SecretFile-windows-x64.zip'
Compress-Archive -LiteralPath $package -DestinationPath $zip -Force
Write-Output "Package: $zip"
Write-Output "Staging directory: $package"
