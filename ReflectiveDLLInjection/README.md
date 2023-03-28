# Password Prompt with Reflective Loader

The same normal Windows DLL as in [PasswordPrompt](../PasswordPrompt),
with with the reflective loader from [github.com/stephenfewer/ReflectiveDLLInjection](https://github.com/stephenfewer/ReflectiveDLLInjection).
built in.
This project is a like a fork of the reflective loader project into which new code was embedded.
For detailed explanations see this [blog post](https://dominicbreuker.com/post/learning_sliver_c2_11_spawndll/).

## Getting started

Open the Visual Studio solution and compile a release binary.
You probably want one for x64.
Copy it over to the Sliver C2 server, then in the Sliver console `spawndll --export NotARefl3ct1veL04d3r /path/to/reflective_dll.x64.dll`.
