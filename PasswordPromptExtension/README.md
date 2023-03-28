# Password Prompt Extension

A Sliver C2 sample extension. 
For detailed explanations see this [blog post](https://dominicbreuker.com/post/learning_sliver_c2_12_extensions/).

## Getting started

Open the Visual Studio solution and compile two release binaries, one for x86 and the other for x64.
Combine it with the [extension.json](./extension.json) file, either into a directory or a `.tar.gz` file.
Copy it over to the Sliver C2 server, then in the Sliver console `extensions install /path/to/extension_dir`.
Restart the Sliver console and you have a new command `PasswordPrompt` which launches the extension.
