#!/bin/bash
if ! command -v dialog &> /dev/null; then
    echo "'dialog' is not installed. Attempting to install..."
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        case "$ID" in
            ubuntu|debian)
                sudo apt update -y &>/dev/null && sudo apt install -y dialog &>/dev/null
                ;;
            fedora)
                sudo dnf install -y dialog &>/dev/null
                ;;
            centos|rhel)
                sudo yum install -y dialog &>/dev/null
                ;;
            arch)
                sudo pacman -Sy --noconfirm dialog &>/dev/null
                ;;
            *)
                echo "Unsupported OS. Please install 'dialog' manually."
                exit 1
                ;;
        esac
    else
        echo "Cannot detect OS. Please install 'dialog' manually."
        exit 1
    fi
    if ! command -v dialog &> /dev/null; then
        echo "Failed to install 'dialog'. Please install it manually."
        exit 1
    fi
fi
rows=20
cols=30
snake="0"
food="*"
empty=" "
score=0
sx=$((rows/2))
sy=$((cols/2))
dir="RIGHT"
snake_body=("$sx,$sy")
start_menu() {
    dialog --clear --title "Snake Game" \
        --menu "Choose an option:" 10 40 2 \
        1 "Start Game" \
        2 "Quit" 2>menu_choice.tmp
    choice=$(<menu_choice.tmp)
    rm -f menu_choice.tmp
    case "$choice" in
        1) return 0 ;;
        2) clear; exit 0 ;;
        *) clear; exit 0 ;;
    esac
}
start_menu
clear
draw_board() {
    clear
    for ((i=0; i<rows; i++)); do
        for ((j=0; j<cols; j++)); do
            cell="$empty"
            for s in "${snake_body[@]}"; do
                [[ "$s" == "$i,$j" ]] && cell="$snake"
            done
            [[ "$i,$j" == "$fx,$fy" ]] && cell="$food"
            echo -n "$cell"
        done
        echo
    done
    echo "Score: $score"
    echo "Controls: WASD or Arrow keys to move. Q to quit."
}
place_food() {
    while :; do
        fx=$((RANDOM % rows))
        fy=$((RANDOM % cols))
        for s in "${snake_body[@]}"; do
            [[ "$s" == "$fx,$fy" ]] && continue 2
        done
        break
    done
}
get_input() {
    read -rsn1 -t 0.1 key
    case "$key" in
        w|W) dir="UP" ;;
        s|S) dir="DOWN" ;;
        a|A) dir="LEFT" ;;
        d|D) dir="RIGHT" ;;
        $'\x1b')
            read -rsn2 -t 0.01 rest
            case "$rest" in
                "[A") dir="UP" ;;
                "[B") dir="DOWN" ;;
                "[C") dir="RIGHT" ;;
                "[D") dir="LEFT" ;;
            esac
            ;;
        q|Q) exit 0 ;;
    esac
}
move_snake() {
    IFS=',' read x y <<< "${snake_body[0]}"
    case "$dir" in
        UP) ((x--)) ;;
        DOWN) ((x++)) ;;
        LEFT) ((y--)) ;;
        RIGHT) ((y++)) ;;
    esac
    if ((x<0 || x>=rows || y<0 || y>=cols)); then
        echo "Game Over. Score: $score" > board.tmp
        dialog --title "Game Over" --msgbox "$(cat board.tmp)" 7 40
        rm -f board.tmp
        exit 0
    fi
    for s in "${snake_body[@]}"; do
        [[ "$s" == "$x,$y" ]] && { echo "Game Over. Score: $score" > board.tmp; dialog --title "Game Over" --msgbox "$(cat board.tmp)" 7 40; rm -f board.tmp; exit 0; }
    done
    if [[ "$x,$y" == "$fx,$fy" ]]; then
        ((score++))
        snake_body=("$x,$y" "${snake_body[@]}")
        place_food
    else
        snake_body=("$x,$y" "${snake_body[@]:0:${#snake_body[@]}-1}")
    fi
}
place_food
while :; do
    draw_board
    get_input
    move_snake
done