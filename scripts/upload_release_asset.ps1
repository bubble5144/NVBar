<#
.SYNOPSIS
  Upload (or replace) a release asset to an existing GitHub Release by tag.

.DESCRIPTION
  This script uploads a file (e.g. bin\NVBar.exe) to an existing GitHub Release identified by tag name.
  If an asset with the same name already exists it will be deleted first and the new asset uploaded.

.NOTES
  Requires a GitHub personal access token with "repo" scope available in the environment variable GITHUB_TOKEN.
  Usage example:
    $env:GITHUB_TOKEN = 'ghp_...'
    .\upload_release_asset.ps1 -Tag v1.2.3 -File ..\bin\NVBar.exe

#>
param(
    [string]$Repo = "bubble5144/NVBar",
    [Parameter(Mandatory=$true)][string]$Tag,
    [string]$File = "..\bin\NVBar.exe",
    [string]$Name = $(if ($PSBoundParameters.ContainsKey('File')) { Split-Path $File -Leaf } else { 'NVBar.exe' })
)

function Write-ErrAndExit($msg) {
    Write-Error $msg
    exit 1
}

if (-not (Test-Path $File)) {
    Write-ErrAndExit "File not found: $File"
}

if (-not $env:GITHUB_TOKEN) {
    Write-ErrAndExit "Environment variable GITHUB_TOKEN is not set. Create a PAT and export it as GITHUB_TOKEN."
}

$headers = @{ Authorization = "token $env:GITHUB_TOKEN"; "User-Agent" = "NVBarUploader" }

# Get release by tag
Write-Output "Looking up release by tag '$Tag' in repo $Repo..."
try {
    $rel = Invoke-RestMethod -Headers $headers -Uri "https://api.github.com/repos/$Repo/releases/tags/$Tag"
} catch {
    Write-ErrAndExit "Failed to find release with tag '$Tag'. Ensure the tag exists and your token has repo access. $_"
}

Write-Output "Found release: $($rel.name) (id $($rel.id))"

# List assets and delete existing asset with same name
$assets = Invoke-RestMethod -Headers $headers -Uri "https://api.github.com/repos/$Repo/releases/$($rel.id)/assets"
$existing = $assets | Where-Object { $_.name -eq $Name }
if ($existing) {
    Write-Output "Deleting existing asset '$Name' (id $($existing.id))..."
    Invoke-RestMethod -Method Delete -Headers $headers -Uri "https://api.github.com/repos/$Repo/releases/assets/$($existing.id)"
}

# Upload new asset
$uploadUrl = $rel.upload_url -replace '\{.*\}$',''
$uri = "$uploadUrl`?name=$Name"
Write-Output "Uploading '$File' as '$Name' to release '$Tag'..."
try {
    $resp = Invoke-RestMethod -Method Post -Headers $headers -ContentType "application/octet-stream" -InFile $File -Uri $uri
} catch {
    Write-ErrAndExit "Upload failed: $_"
}

Write-Output "Upload successful. Asset URL: $($resp.browser_download_url)"
Write-Output "Release page: $($rel.html_url)"
