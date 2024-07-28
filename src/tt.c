// #include "defs.h"
// #include <assert.h>
//
// // clear table
// void initTT() {
//     for(int i=0;i<TT_SIZE;++i) {
//         tt[i].hash = 0ull;
//     }
// }
//
// int probeTT(ULL hash, int alpha, int beta, int depth) {
//     TT *entry = &tt[hash % TT_SIZE];
//
//     if(entry->hash != hash) {
//         return TT_EMPTY;
//     }
//
//     return entry->flag;
// }
//
// void writeTT(ULL hash, int alpha, int beta, int depth) {
//     tt[hash % TT_SIZE].hash = hash;
//     tt[hash % TT_SIZE].score = alpha;
// }
