#!/usr/bin/env python3

BUFFER = 50
MINIMUM = 10
MOVES = 40
SYSTEM_OVERHEAD = 50

WTIME: int = 5000
WINC: int = 100

clock: int = WTIME
move = 1
target_sum = 0
remaining_moves = 40

# ###### TIME LOGIC ######
def timeLogic():
    global remaining_moves

    target:int = (clock + (remaining_moves - 1) * WINC) // remaining_moves

    hard_limit = clock // 5
    target = min(target, hard_limit) - BUFFER
    target = max(MINIMUM, target)
    remaining_moves = max(MOVES - move, 20)

    return target


# ###### SIMULATION ######
for i in range(MOVES):
    if clock <= 0:
        print(f"TIMEOUT at move {move}")
        break

    target: int = timeLogic()
    if i % 3 == 0 or i == 0 or i == MOVES - 1:
        print(f"#{move}: {target}ms - {clock}")

    target_sum += target + SYSTEM_OVERHEAD
    move += 1
    clock += WINC - target - SYSTEM_OVERHEAD

# ###### RESULTS ######
game_time = WTIME + (MOVES) * WINC
print(f"Simulated {MOVES} moves, Remaining time on clock: {clock}")
print(f"Time used: {target_sum}, Game time: {game_time}, Usage {round(target_sum * 100 / game_time)}%, Average time: {round(target_sum / MOVES)}")