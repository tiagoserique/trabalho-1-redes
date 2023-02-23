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
            try:
                rebuildData += chr(grid[i][j])
            except Exception:
                rebuildData += '*'

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
    l[random.randrange(0, size)] = random.choice(string.ascii_letters)
    l[random.randrange(0, size)] = random.choice(string.ascii_letters)
    s = ''.join(l)
    return s

def checkError(grid, ierr, jerr):
    line = 0
    for i in range(7):
        line += grid[ierr][i]
    
    line -= grid[ierr][7]

    col = 0
    for i in range(7):
        col += grid[i][jerr]

    col -= grid[7][jerr]

    # print(ierr, jerr)
    # print(line, col)

    if col == 0 or line == 0:
        return False
    return True

def checkCantCorrect(iError, jError):
    print(iError, jError)
    return False

def correctGrid(grid):
    # recebe uma matriz com erro e tenta corrigir

    iError = []
    jError = []

    # Acha os erros em linha
    for i in range(7):
        s = 0
        for j in range(7):
            s += grid[i][j]
        if s != grid[i][7]:
            iError += [i]

    # acha os erros em coluna
    for j in range(7):
        s = 0
        for i in range(7):
            s += grid[i][j]
        if s != grid[7][j]:
            jError += [j]

    if iError==[] or jError==[]: return grid, True

    for ierr in iError:
        for jerr in jError:
            if checkError(grid, ierr, jerr):
                start = grid[ierr][7]
                for i in range(7):
                    if i == jerr: continue
                    start -= grid[ierr][i]
                grid[ierr][jerr] = start

    # Acha os erros em linha
    for i in range(7):
        s = 0
        for j in range(7):
            s += grid[i][j]
        if s != grid[i][7]:
            return grid, False

    # acha os erros em coluna
    for j in range(7):
        s = 0
        for i in range(7):
            s += grid[i][j]
        if s != grid[7][j]:
            return grid, False

    return grid, True

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

    print("Dados originais: \n", dataRaw)
    # printGrid(grid)
    calcParity(grid)
    # printGrid(grid)
    stream = gridToStream(grid)
    streamOriginal = stream
    print("Dados a serem enviados: \n", stream)
    stream = addErrorToStream(stream)
    print("Dados recebidos com erro: \n", stream)

    # check errors
    pos = []
    for i in range(len(streamOriginal)):
        if stream[i] != streamOriginal[i]:
            pos += [i]
    for p in pos:
        print(p, " "*(p-(2 if p < 10 else 3)), stream[p])

    grid = streamtoGrid(stream)
    grid, cond = correctGrid(grid)
    if cond:
        print("Conseguiu arrumar")
    else:
        print("Impossivel arrumar, reenviar")
    print("Frase resultante: \n", rebuildData(grid))
