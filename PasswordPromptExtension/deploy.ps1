$compress = @{
  Path = "C:/playground/PasswordPromptExtension/bin/extension.json", "C:/playground/PasswordPromptExtension/bin/*.dll"
  CompressionLevel = "Fastest"
  DestinationPath = "C:/share/PasswordPromptExtension.zip"
}
Compress-Archive -Force @compress