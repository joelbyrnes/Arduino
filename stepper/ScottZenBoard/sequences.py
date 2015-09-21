
class Sequence():
    def __init__(self, name, sequence):
        self.name = name
        self.sequence = sequence
        self.elems = len(sequence)
        self.intervals = sum(self.sequence)

    def sequencePosAt(self, time):
        return time % self.intervals

    def orientationAt(self, time):
        remainder = self.sequencePosAt(time)
        for n in range(0, self.elems):
            if sum(self.sequence[0:n+1]) > remainder:
                return n % 4

sequenceA = Sequence('A', [3, 4, 5, 6, 7, 8, 3])
sequenceB = Sequence('B', [4, 5, 6, 7, 8, 3, 4])
sequenceC = Sequence('C', [5, 6, 7, 8, 3, 4, 5])
sequenceD = Sequence('D', [6, 7, 8, 3, 4, 5, 6])
sequenceE = Sequence('E', [7, 8, 3, 4, 5, 6, 7])
sequenceF = Sequence('F', [8, 3, 4, 5, 6, 7, 8])

# sequenceA = Sequence('A', [1, 2, 3, 4, 5, 6, 1])
# sequenceB = Sequence('B', [2, 3, 4, 5, 6, 1, 2])
# sequenceC = Sequence('C', [3, 4, 5, 6, 1, 2, 3])
# sequenceD = Sequence('D', [4, 5, 6, 1, 2, 3, 4])
# sequenceE = Sequence('E', [5, 6, 1, 2, 3, 4, 5])
# sequenceF = Sequence('F', [6, 1, 2, 3, 4, 5, 6])

sequences = [sequenceA, sequenceB, sequenceC, sequenceD, sequenceE, sequenceF]

for c in sequences:
    print c.sequence, sum(c.sequence)

combos = set()

last_time_found = 2
count = 0
all_combos_found = False

for i in range(0, 1000000):

    orientations = map(lambda c: c.orientationAt(i), sequences)

    if not all_combos_found:
        combos.add(str(orientations))

    # for c in sequences:
    #     print "%s at time %d : %s" % (c.name, i, c.posAt(i))
    # print ''

    sequencePoss = map(lambda c: c.sequencePosAt(i), sequences)
    # print "time %d, \tpositions %s,\tsequence IDs %s" % (i, poss, sequenceIds)

    # print map(lambda c: c.posAt(i), sequences)
    if i > last_time_found and sum(orientations) == 0:
        count += 1
        last_time_found = i + 2
        # print "All orientations at pos 0 at time %d (%d times)" % (i, count)
        print "time %d, \torientations %s,\tsequence positions %s" % (i, orientations, sequencePoss)

    if sum(sequencePoss) == 0:
        print "*** All sequence positions at 0 at time %d" % (i)

    if not all_combos_found and len(combos) == 4096:
        print "**** all patterns found by %d" % i
        all_combos_found = True
        # break

print "combos:", len(combos)
