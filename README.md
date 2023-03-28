# Sliver Samples

This is just a companion repository for this
[blog post series](https://dominicbreuker.com/post/learning_sliver_c2_01_installation/#series-overview)
on Sliver C2.
Currently contains a "Hello World" program which opens a password prompt dialog to get user credentials,
implemented in different ways.
For example, you could use them to infect a representative server or workstation in your company with a Sliver implant,
run these programs and see if any alerts are created.
Or you use them as templates for your own development.

Currently there is:
- [PasswordPrompt](./PasswordPrompt):
  A normal Windows DLL.
  Use it with the Sliver `sideload` command.
  See also [here](https://dominicbreuker.com/post/learning_sliver_c2_10_sideload/).
- [PasswordPromptExtension](./PasswordPromptExtension):
  An extension which will be reflectively loaded into the implant process.
  Use it with the Sliver `extensions` command.
  See also [here](https://dominicbreuker.com/post/learning_sliver_c2_12_extensions/).
