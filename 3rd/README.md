# 3rd Party Code

Add all libraries like SDL, SDL_ttf, ... here.

## Shallow

Add shallow submodules like:

`git submodule add --depth 1 <repo> <path>`
`git config -f .gitmodules submodule.<path>.shallow true`

You can unshalloe them with

`git config -f .gitmodules submodule.<path>.shallow false`
`git submodule update <path>`


