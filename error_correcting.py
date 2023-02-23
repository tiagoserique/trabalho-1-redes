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

    grid = [[0 for __ in range(7)] for _ in range(7)]
    for i in range(7):
        for j in range(7):
            grid[i][j] = paddedData[i*7+j]

    return dataRaw, data, paddedData, grid

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
    for j in range(7):
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
    l[random.randrange(0, len(l)-9)] = random.choice(list(map(chr, range(256))))
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
    print()
    print("Print grid: ")
    for g in grid:
        for gg in g:
            print(str(gg).ljust(3), ' ', end='')
        print()
    print()

if __name__ == "__main__":

    dataRaw, data, paddedData, grid = getInput()

    print("Dados originais:", dataRaw)
    printGrid(grid)
    calcParity(grid)
    printGrid(grid)
    stream = gridToStream(grid)
    streamOriginal = stream
    print("Dados a serem enviados: \n", stream)
    stream = addErrorToStream(stream)
    print("Dados recebidos com erro: \n", stream)
    for i in range(len(streamOriginal)):
        if stream[i] != streamOriginal[i]:
            pos = i
            break
    print(pos, " "*(pos-(2 if pos < 10 else 3)), stream[pos])

    grid = streamtoGrid(stream)
    grid = correctGrid(grid)
    print("Frase arrumada: \n", rebuildData(grid))

    # print(grid)
    # print(stream)