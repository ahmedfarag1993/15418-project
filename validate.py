import sys

def validateBoruvka(filename):
    mismatchedEdges = set()
    mstEdges = set()
    isSequential = True
    f = open(filename)

    parEdgeCount = 0
    for line in f.readlines():
        line = line.strip()
        if line == "end sequential":
            isSequential = False
            continue
        if "time" in line:
            continue
        elems = line.split(',')
        e = (int(elems[0]), int(elems[1]))
        eRev = (int(elems[1]), int(elems[0]))
        if isSequential:
            mstEdges.add(e)
            mstEdges.add(eRev)
        else:
            parEdgeCount += 2
            if e not in mstEdges and eRev not in mstEdges:
                mismatchedEdges.add(e)
            
    f.close()
    return (mismatchedEdges, parEdgeCount == len(mstEdges))

filename = sys.argv[1]
print validateBoruvka(filename)
