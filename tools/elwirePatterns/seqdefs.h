/*
* EL wire pattern sequences defintions
*/

#ifndef SEQ_DEFS_H
#define SEQ_DEFS_H

#define NUM_SEQUENCES           8
#define MAX_SEQUENCE_LENGTH     64

char sequences[NUM_SEQUENCES][MAX_SEQUENCE_LENGTH][NUM_EL_WIRES] = {
    {                   // Sequence #0
        "        ",
        "   #   #",
        "  ##  ##",
        " ### ###",
        "########",
        " ### ###",
        "  ##  ##",
        "   #   #",
    },
    {                   // Sequence #1
        "        ",
        "########",
    },
    {                   // Sequence #2
        "        ",
        "       #",
        "      ##",
        "     ###",
        "    ####",
        "   #####",
        "  ######",
        " #######",
        "########",
        " #######",
        "  ######",
        "   #####",
        "    ####",
        "     ###",
        "      ##",
        "       #",
    },
    {                   // Sequence #3
        "        ",
        "       #",
        "      # ",
        "     #  ",
        "    #   ",
        "   #    ",
        "  #     ",
        " #      ",
        "#       ",
        " #      ",
        "  #     ",
        "   #    ",
        "    #   ",
        "     #  ",
        "      # ",
        "       #",
        "#       ",
        " #      ",
        "  #     ",
        "   #    ",
        "    #   ",
        "     #  ",
        "      # ",
        "       #",
        "      # ",
        "     #  ",
        "    #   ",
        "   #    ",
        "  #     ",
        " #      ",
        "#       ",
    },
    {                   // Sequence #4
        "        ",
        "#      #",
        "##    ##",
        "###  ###",
        "########",
        "###  ###",
        "##    ##",
        "#      #",
        "        ",
        "#      #",
        " #    # ",
        "  #  #  ",
        "   ##   ",
        "  #  #  ",
        " #    # ",
        "#      #",
        "########",
        " ###### ",
        "  ####  ",
        "   ##   ",
        "  ####  ",
        " ###### ",
        "########",
    },
    {                   // Sequence #5
        "# # # # ",
        " # # # #",
    },
    {                   // Sequence #6
        "##  ##  ",
        " ##  ## ",
        "  ##  ##",
        "#  ##  #",
    },
    {                   // Sequence #7
        "########",
        "        ",
        "########",
        "        ",
        "########",
        "        ",
        "########",
        "        ",
        "####    ",
        "    ####",
        "####    ",
        "    ####",
        "####    ",
        "    ####",
        "####    ",
        "    ####",
        "##  ##  ",
        "  ##  ##",
        "##  ##  ",
        "  ##  ##",
        "##  ##  ",
        "  ##  ##",
        "##  ##  ",
        "  ##  ##",
        " # # # #",
        "# # # # ",
        " # # # #",
        "# # # # ",
        " # # # #",
        "# # # # ",
        " # # # #",
        "# # # # ",
    },
};


#endif /*SEQ_DEFS_H*/