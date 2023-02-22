import random
import math
import string

def getInput():
    # dataRaw = str(input())
    # dataRaw = "abc"
    dataRaw = "FraseDeTesteMuitoLoucaQueVouUsarDeTeste"
    size = len(dataRaw)
    DATA_LEN = 49
    if len(dataRaw) > DATA_LEN:
        dataRaw = dataRaw[:DATA_LEN]

    # # error = int(input())
    # error = 0
    # if 0 > error > DATA_LEN or error > len(dataRaw):
    #     error = random.randrange(0, len(dataRaw))
    data = [ord(l) for l in dataRaw]

    paddedData = data + [0 for _ in range(49-len(data))]

    return dataRaw, data, paddedData

def rebuildData(grid):
    rebuildData = ''
    for i in range(7):
        for j in range(7):
            rebuildData += chr(grid[i][j])

    return rebuildData

def calcParity(grid):
    for i in range(7):
        s = 0
        for j in range(7):
            s += grid[i][j]
        grid[i] += [s]

    line = []
    for j in range(8):
        s = 0
        for i in range(7):
            s += grid[i][j]
        line += [s]
    grid += [line+[0]]

    # s = 0
    # for i in range(7):
    #     s += grid[7][i]
    # grid[7][7] += s

def gridToStream(grid):
    stream = ''
    for i in range(8):
        for j in range(8):
            stream += chr(grid[i][j])
    return stream

def streamtoGrid(stream):
    grid = [[0 for __ in range(8)] for _ in range(8)]
    for i in range(8):
        for j in range(8):
            grid[i][j] = ord(stream[i*8+j])
    return grid

def addErrorToStream(stream):
    l = [l for l in stream]
    l[random.randrange(0, len(l))] = random.choice(list(map(chr, range(256))))
    s = ''.join(l)
    return s

def correctGrid(grid):
    # printGrid(grid)
    iError = -1
    jError = -1
    for i in range(7):
        s = 0
        for j in range(7):
            s += grid[i][j]
        if s != grid[i][7]:
            iError = i

    for j in range(7):
        s = 0
        for i in range(7):
            s += grid[i][j]
        if s != grid[7][j]:
            jError = j

    # print(iError, jError)
    if iError==-1 or jError==-1: return grid

    start = grid[iError][7]
    for i in range(7):
        if i == jError: continue
        start -= grid[iError][i]
    # print(start)
    grid[iError][jError] = start

    # printGrid(grid)
    return grid

def printGrid(grid):
    for g in grid:
        for gg in g:
            print(gg, ' ', end='')
        print()

if __name__ == "__main__":

    dataRaw, data, paddedData = getInput()

    grid = [[0 for __ in range(7)] for _ in range(7)]
    for i in range(7):
        for j in range(7):
            grid[i][j] = paddedData[i*7+j]

    print(dataRaw)
    calcParity(grid)
    stream = gridToStream(grid)
    stream = addErrorToStream(stream)
    grid = streamtoGrid(stream)
    print(rebuildData(grid))
    grid = correctGrid(grid)
    print(rebuildData(grid))

    # print(dataRaw)
    # print(grid)
    # print(stream)