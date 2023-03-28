# Password Prompt 

A normal Windows DLL.
For detailed explanations see this [blog post](https://dominicbreuker.com/post/learning_sliver_c2_10_sideload/).

## Getting started

Open the Visual Studio solution and compile a release binary.
You probably want one for x64.
Copy it over to the Sliver C2 server, then in the Sliver console `sideload /path/to/PasswordPrompt.dll`.
