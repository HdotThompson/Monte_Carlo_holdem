#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <random>
#include <algorithm>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include "arrays.h"

using namespace std;

vector<string> init_deck() {
    string str_suits = "cdhs";
    string str_ranks = "23456789TJQKA";
    string card;
    vector<string> deck;

    vector<char> suits(str_suits.begin(), str_suits.end());
    vector<char> ranks(str_ranks.begin(), str_ranks.end());

    for (char card_rank: ranks) {
        for (char card_suit: suits) {
            card = card_rank;
            card += card_suit;
            deck.push_back(card);
        }
    }

    return deck;
}

map<string, int> init_hashtable(vector<string> deck) {
    vector<int> hashes;
    map<string, int> hashmap;
    int hash;

    int primes [] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41};

    for (int i=0;i<13;i++) {
        for (int j=0;j<4;j++) {
            hash = (1 << (i + 16)) | (i << 8) | 1 << (j + 12) | primes[i];
            hashes.push_back(hash);
        }
    }

    for (size_t i=0;i<deck.size();i++)
        hashmap[deck[i]] = hashes[i];

    return hashmap;
}

static unsigned hash_function(unsigned u) {
    unsigned a, b, r;

    u += 0xe91aaa35;
    u ^= u >> 16;
    u += u << 8;
    u ^= u >> 4;
    b  = (u >> 8) & 0x1ff;
    a  = (u + (u << 2)) >> 19;
    r  = a ^ hash_adjust[b];
    return r;
}

unsigned short get_combination_result(int *hand) {
    int c1, c2, c3, c4, c5, q;

    c1 = *hand++;
    c2 = *hand++;
    c3 = *hand++;
    c4 = *hand++;
    c5 = *hand;

    q = (c1 | c2 | c3 | c4 | c5) >> 16;
    short s;

    if (c1 & c2 & c3 & c4 & c5 & 0xf000)
        return flushes[q];

    if ((s = unique5[q]))
        return s;

    q = (c1 & 0xff) * (c2 & 0xff) * (c3 & 0xff) * (c4 & 0xff) * (c5 & 0xff);
    return hash_values[hash_function(q)];
    return 1;
}

unsigned short get_hand_result(vector<string> hand, map<string, int> hashmap) {
    int i, j, subhand[5];
    vector<int> hash_hand;
    unsigned short q, best = 9999;

    for(string card: hand)
        hash_hand.push_back(hashmap[card]);

    for (i = 0; i < 21; i++)
    {
        for (j = 0; j < 5; j++)
            subhand[j] = hash_hand[ perm7[i][j] ];
        q = get_combination_result(subhand);
        if (q < best)
            best = q;
    }
    return best;
}

void simulate(vector<string> hand1, vector<string> hand2, int iter_number) {
    vector<string> community, cur_hand;
    int score1=0, score2=0, win_count1=0, win_count2=0, tie_count = 0;

    vector<string> deck = init_deck();
    map<string, int> hashmap = init_hashtable(deck);
    auto rng = default_random_engine (time(0));

    for (string card: hand1)
        deck.erase(remove(deck.begin(), deck.end(), card), deck.end());
    for (string card: hand2)
        deck.erase(remove(deck.begin(), deck.end(), card), deck.end());

    for (int i=0;i<iter_number;i++) {
        shuffle(begin(deck), end(deck), rng);

        for (int j=0;j<5;j++)
            community.push_back(deck[j]);

        cur_hand = community;
        cur_hand.insert(cur_hand.end(), hand1.begin(), hand1.end());
        score1 = get_hand_result(cur_hand, hashmap);

        cur_hand = community;
        cur_hand.insert(cur_hand.end(), hand2.begin(), hand2.end());
        score2 = get_hand_result(cur_hand, hashmap);

        if(score1 < score2)
            win_count1++;
        else if(score1 == score2)
            tie_count++;
        else
            win_count2++;

        community.clear();
    }

    cout<<"first hand winrate: "<<float(win_count1) / float(iter_number) * 100<<'\n';
    cout<<"Second hand winrate: "<<float(win_count2) / float(iter_number) * 100<<'\n';
    cout<<"Tie chance: "<<float(tie_count) / float(iter_number) * 100<<'\n';
}

int main() {
    vector<string> hand1 {"2c", "Qc"};
    vector<string> hand2 {"Th", "Jd"};

    simulate(hand1, hand2, 10000);

    return 0;
}
