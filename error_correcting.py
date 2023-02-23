import random
import math
import string

size = 0

def getInput():
    # Pega a frase a ser enviada
    # dataRaw = str(input())
    dataRaw = "FraseDeTesteMuitoLoucaQueVouUsarDeTeste"
    global size
    size = len(dataRaw)
    DATA_LEN = 49
    if len(dataRaw) > DATA_LEN:
        dataRaw = dataRaw[:DATA_LEN]

    # Converte em inteiros
    data = [ord(l) for l in dataRaw]

    # completa o tamanho
    paddedData = data + [0 for _ in range(49-len(data))]

    # Gera a matriz
    grid = [[0 for __ in range(7)] for _ in range(7)]
    for i in range(7):
        for j in range(7):
            grid[i][j] = paddedData[i*7+j]

    return dataRaw, data, paddedData, grid

def rebuildData(grid):
    # transforma a matriz em dados legiveis
    rebuildData = ''
    for i in range(7):
        for j in range(7):
            rebuildData += chr(grid[i][j])

    return rebuildData

def calcParity(grid):
    # calcula a paridade
    for i in range(7):
        s = 0
        for j in range(7):
            s += grid[i][j]
        # Adiciona a paridade no fim da linha
        grid[i] += [s]

    line = []
    for j in range(7):
        s = 0
        for i in range(7):
            s += grid[i][j]
        # Adiciona a paridade no fim da coluna
        line += [s]
    grid += [line+[0]]

def gridToStream(grid):
    # Transforma uma matriz numa sequencia de char
    stream = ''
    for i in range(8):
        for j in range(8):
            stream += chr(grid[i][j])
    return stream

def streamtoGrid(stream):
    # Transforma a sequencia de char em uma matriz
    grid = [[0 for __ in range(8)] for _ in range(8)]
    for i in range(8):
        for j in range(8):
            grid[i][j] = ord(stream[i*8+j])
    return grid

def addErrorToStream(stream):
    # Adiciona um erro aleatorio na sequencia
    l = [l for l in stream]
    l[random.randrange(0, size)] = random.choice(list(map(chr, range(256))))
    s = ''.join(l)
    return s

def correctGrid(grid):
    # recebe uma matriz com erro e tenta corrigir

    iError = -1
    jError = -1

    # Acha os erros em linha
    for i in range(7):
        s = 0
        for j in range(7):
            s += grid[i][j]
        if s != grid[i][7]:
            iError = i

    # acha os erros em coluna
    for j in range(7):
        s = 0
        for i in range(7):
            s += grid[i][j]
        if s != grid[7][j]:
            jError = j

    if iError==-1 or jError==-1: return grid

    start = grid[iError][7]
    for i in range(7):
        if i == jError: continue
        start -= grid[iError][i]
    grid[iError][jError] = start

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