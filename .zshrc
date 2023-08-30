#!/bin/sh
[ -f "${XDG_DATA_HOME:-$HOME/.local/share}/zap/zap.zsh" ] && source "${XDG_DATA_HOME:-$HOME/.local/share}/zap/zap.zsh"

## HISTORY
HISTFILE=~/.zsh_history

## SOURCE
plug "$HOME/.config/zsh/aliases.zsh"
# plug "$HOME/.config/zsh/exports.zsh"

## PLUGINS
plug "zsh-users/zsh-autosuggestions"
plug "zap-zsh/supercharge"
# plug "zap-zsh/zap-prompt"
plug "zsh-users/zsh-syntax-highlighting"
plug "zap-zsh/fzf"

## PROMPT
PS1="%B%{$fg[blue]%}[%{$fg[yblue]%}%n%{$fg[blue]%}@%{$fg[blue]%}%M %{$fg[blue]%}%1d%{$fg[blue]%}]λ%b "

bindkey '^[[H' beginning-of-line
bindkey '^[[4~' end-of-line
bindkey  "^[[3~"  delete-char
bindkey -s '^f' 'se'
bindkey -s '^[OQ' '. ranger'
bindkey "^[[1;5C" forward-word
bindkey "^[[1;5D" backward-word

export PATH=~/scripts:~/git/rofi-network-manager:$PATH

## COMPLETION
autoload -Uz compinit
compinit
