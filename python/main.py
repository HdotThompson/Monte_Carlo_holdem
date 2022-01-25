from data import *
import itertools
import random
import timeit

_DECK = [(1 << (rank + 16)) | (rank << 8) | 1 << (suit + 12) | PRIMES[rank] for rank, suit in
         itertools.product(range(13), range(4))]
SUITS = 'cdhs'
RANKS = '23456789TJQKA'
DECK = [''.join(s) for s in itertools.product(RANKS, SUITS)]
LOOKUP = dict(zip(DECK, _DECK))

def hash_function(x):
    x += 0xe91aaa35
    x ^= x >> 16
    x += x << 8
    x &= 0xffffffff
    x ^= x >> 4
    b = (x >> 8) & 0x1ff
    a = (x + (x << 2)) >> 19
    r = (a ^ HASH_ADJUST[b]) & 0x1fff
    return HASH_VALUES[r]


def get_combination_result(hand):
    c1, c2, c3, c4, c5 = (LOOKUP[x] for x in hand)
    q = (c1 | c2 | c3 | c4 | c5) >> 16
    if (0xf000 & c1 & c2 & c3 & c4 & c5):
        return FLUSHES[q]
    s = UNIQUE_5[q]
    if s:
        return s
    p = (c1 & 0xff) * (c2 & 0xff) * (c3 & 0xff) * (c4 & 0xff) * (c5 & 0xff)
    return hash_function(p)


def get_hand_result(hand):
    return min(get_combination_result(x) for x in itertools.combinations(hand, 5))


def simulate(hand1, hand2, iter_number):
    # prepare shuffle a deck
    deck = list(DECK)
    win_count1, win_count2, tie_count = (0, 0, 0)

    for item in hand1 + hand2:
        deck.remove(item)

    for i in range(iter_number):
        random.shuffle(deck)
        # draw 5 cards
        community = deck[:5]
        # evaluate the hands
        score1 = get_hand_result(community + hand1)
        score2 = get_hand_result(community + hand2)
        # display the winning hand
        if score1 < score2:
            win_count1 += 1
        elif score1 == score2:
            tie_count += 1
        else:
            win_count2 += 1

    print("first hand winrate: ", (win_count1 / iter_number) * 100)
    print("Second hand winrate: ", (win_count2 / iter_number) * 100)
    print("Tie chance: ", (tie_count / iter_number) * 100)


if __name__ == '__main__':
    start = timeit.default_timer()
    simulate(['2c', 'Qc'], ['Th', 'Jd'], 10000)
    end = timeit.default_timer()
    print(end - start)
