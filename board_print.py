done = False
lines = []
while not done:
    l = input()
    if l == 'q':
        done = True
    lines.append(l)

text = ''.join(line.strip() for line in lines)

board = []
while text:
    if text.startswith('BoardCell'):
        start = text.find('<') + 1
        end = text.find('>')
        params = text[start:end].split(',')
        car = params[0].strip()
        direction = params[1].strip()
        amount = int(params[2].strip())
        board.append((car, direction, amount))
    text = text[1:]


# Print 6x6 board. Show the car types.
for i in range(6):
    for j in range(6):
        ch = board[j + 6 * i][0]
        if ch == 'EMPTY': ch = '.'
        print(ch, end=' ')
    print()
