alias se="du -a ~/.zshrc ~/scripts/* ~/git/dwm/* ~/git/st/* ~/git/dwmblocks ~/.config/* | awk '{print $2}' | fzf | xargs -r nvim ;"
alias cleanup_packages='set -x; sudo pacman -Rs $(pacman -Qdtq)'
