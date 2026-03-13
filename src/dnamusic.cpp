
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "../../midi_writer_cpp/include/midi_writer.h"

const char* dnalett = "ATGC";
const int dnanotes[4] = {69, 64, 67, 60};
const char genetic_code[4][4][5] =
    {
        { "KNKN", "IIMI", "RSRS", "TTTT" },
        { "\0Y\0Y", "LFLF", "\0CWC", "SSSS" },
        { "EDED", "VVVV", "GGGG", "AAAA" },
        { "QHQH", "LLLL", "RRRR", "PPPP" }
    };
const int bassnotes[26][6] =                       // from https://doi.org/10.1093/bioinformatics/12.3.251
    {   {48,48,0,0,0,0}, {0,0,0,0,0,0}, {48,48,0,0,0,0}, {33,33,0,0,41,41}, {33,33,0,0,36,36},
        {38,38,0,0,36,36}, {48,48,0,0,0,0}, {40,40,33,36,38,41}, {36,36,0,0,43,43}, {0,0,0,0,0,0},
        {33,33,36,36,41,40}, {36,36,0,0,43,43}, {36,36,0,0,0,0}, {33,33,0,0,0,0}, {0,0,0,0,0,0},
        {0,0,0,0,0,0}, {33,33,0,0,0,0}, {33,33,40,40,41,41}, {45,45,0,0,0,0}, {33,33,0,0,36,36},
        {0,0,0,0,0,0}, {36,36,0,0,43,43}, {38,38,36,36,33,33}, {0,0,0,0,0,0}, {38,38,36,36,33,33}, {0,0,0,0,0,0}
     };
const int notedur = 480;
const int halfdur = notedur/2;

using namespace std;

int main(int argc, char** argv)
{
    char sequence[65536];
    char outfname[256];
    if (argc > 1) strcpy(sequence, argv[1]);
    else strcpy(sequence, "ATGGGGACTGGAAATGACACCACTGTGGTAGAGTTTACTCTTTTGGGGTTATCTGAGGATACTACAGTTTGTGCTATTTTATTTCTTGTGTTTCTAGGAATTTATGTTGTCACCTTAATGGGTAATATCAGCATAATTGTATTGATCAGAAGAAGTCATCATCTTCATACACCCATGTACATTTTCCTCTGCCATTTGGCCTTTGTAGACATTGGGTACTCCTCATCAGTCACACCTGTCATGCTCATGAGCTTCCTAAGGAAAGAAACCTCTCTCCCTGTTGCTGGTTGTGTGGCCCAGCTCTGTTCTGTAGTGACGTTTGGTACGGCCGAGTGCTTCCTGCTGGCTGCCATGGCCTATGATCGCTATGTGGCCATCTGCTCACCCCTGCTCTACTCTACCTGCATGTCCCCTGGAGTCTGCATCATCTTAGTGGGCATGTCCTACCTGGGTGGATGTGTGAATGCTTGGACATTCATTGGCTGCTTATTAAGACTGTCCTTCTGTGGGCCAAATAAAGTCAATCACTTTTTCTGTGACTATTCACCACTTTTGAAGCTTGCTTGTTCCCATGATTTTACTTTTGAAATAATTCCAGCTATCTCTTCTGGATCTATCATTGTGGCCACTGTGTGTGTCATAGCCATATCCTACATCTATATCCTCATCACCATCCTGAAGATGCACTCCACCAAGGGCCGCCACAAGGCCTTCTCCACCTGCACCTCCCACCTCACTGCAGTCACTCTGTTCTATGGGACCATTACCTTCATTTATGTGATGCCCAAGTCCAGCTACTCAACTGACCAGAACAAGGTGGTGTCTGTGTTCTACACCGTGGTGATTCCCATGTTGAACCCCCTGATCTACAGCCTCAGGAACAAGGAGATTAAGGGGGCTCTGAAGAGAGAGCTTAGAATAAAAATATTTTCTTGA");      // OR5P3
    if (argc > 2) strcpy(outfname, argv[2]);
    else strcpy(outfname, "test.mid");
    int octave = 4;

    MidiWriter mw;
    int melody = mw.add_track();
    int bassline = mw.add_track();
    mw.add_time_signature(melody, 0, 3, 4);
    mw.add_time_signature(bassline, 0, 3, 4);
    mw.add_bpm(melody, 0, 180);
    mw.set_channel(0, 0);
    mw.set_channel(1, 33);
    int i, j, l, x, y, z;

    for (i=0; sequence[i]; i++) sequence[i] = sequence[i] & 0xdf;
    int seqlen = i, lndur = notedur, offset = 0, borrow = 0;

    for (i=0; sequence[i]; i++)
    {
        lndur = notedur - borrow;
        borrow = 0;
        if (sequence[i] == 'U') sequence[i] = 'T';
        const char* li = strchr(dnalett, sequence[i]);
        if (!li)
        {
            cerr << "Unknown sequence letter " << sequence[i] << endl << flush;
            throw 0xbadbad;
        }
        j = li - dnalett;

        int imod3 = i%3;
        if (i)
        {
            if (sequence[i] == 'A' && sequence[i-1] == 'C' && octave > 3) octave--;
            if (sequence[i] == 'C' && sequence[i-1] == 'A' && octave < 7) octave++;

            if (imod3 < 2 && sequence[i] == sequence[i+1])
            {
                borrow = lndur/2;
                lndur += borrow;
            }
            // else if (imod3 && sequence[i] == sequence[i-1]) lndur *= 0.5;
        }

        if (i == seqlen-2) lndur = notedur*2;
        if (i == seqlen-1) lndur = notedur*6;

        mw.add_note(melody, 0, notedur*i+offset, lndur, dnanotes[j] + 12 * (octave-4), 64);

        if (!imod3)
        {
            x = j;
            if (!sequence[i+1] || !sequence[i+2])
            {
                cerr << "Sequence length must be a multiple of 3." << endl << flush;
                throw 0xbadbad;
            }
            li = strchr(dnalett, sequence[i+1]);
            y = li - dnalett;
            li = strchr(dnalett, sequence[i+2]);
            z = li - dnalett;
            char residue = genetic_code[x][y][z];
            cout << residue;
            j = residue - 'A';
            for (l=0; l<6; l++)
            {
                if (!bassnotes[j][l]) continue;
                if (!(l&1) && bassnotes[j][l] == bassnotes[j][l+1])
                {
                    mw.add_note(bassline, 1, notedur*i+halfdur*l+offset, notedur, bassnotes[j][l], 64);
                    l++;
                }
                else
                {
                    mw.add_note(bassline, 1, notedur*i+halfdur*l+offset, halfdur, bassnotes[j][l], 64);
                }
            }
        }

        offset += lndur-notedur;
    }
    cout << endl;

    mw.save(outfname);
    cout << "Saved " << outfname << endl;
    return 0;
}