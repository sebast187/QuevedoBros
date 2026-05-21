#ifndef LEVELS_H
#define LEVELS_H

#define MAP_WIDTH 120
#define MAP_HEIGHT 18

// 8 Main Levels + 3 Secret Levels
#define NUM_MAIN_LEVELS 8
#define MAX_LEVELS 11

// Legend: 
// P=Player, #=Ground (Dirt), B=Breakable Brick, ?=Mystery, X=Hard Block (Metal), 
// C=Coin, ^=Spikes, F=Flagpole, 
// WARP PIPES: W = Secret 1, V = Secret 2, U = Secret 3
// Enemies: E=Waddler, H=Hopper, D=Dropper

// Player dist (spaces " "): 3 high - 4 deep, 2 high - 5 deep, 1 high - 6 deep, 0 high - 7 deep, -2 down - 7 deep
// Max jump height is 5 blocks, max cliff height is 4 blocks

// Max level height (camera view) is 15 spaces and max level depth is 123 spaces
// The camera prefers the upper part, so levels with 16+ spaces of height will not show the blocks in the bottom that are in the 16th+ row

// To make and build the game via visual studio terminal use these commands:
// Use terminal path to enter the correct folder: cd "C:\Path\To\Quevedo Bros"
// Make app for Mac: make clean && make
// To run the game after building, use: ./build/quevedo_bros
// Make app for Windows: make clean -f Makefile.win && make -f Makefile.win

// To edit the game version, edit the string below CFBundleVersion in the Makefile

const char* GAME_LEVELS[MAX_LEVELS][MAP_HEIGHT] = {
    // LEVEL 1: "Brick Breaker Lane" (Basic introduction to mechanics, no enemies)
    {
        "",
        "",
        "",
        "",
        "",
        "",
        "                                                                                     CCC",
        "                                 ?                                                   B?B          BB",
        "                                                                                                  ?B",
        "               C               C   C             C                                                               ",
        "               ?               B B B            B B            ?                  B B                     B     F",
        "                                                                               B  B B  B         BBBC    B      #",
        "P                     E      C                                 E             B B EB B EB B       BBBBBBBBC      #",
        "################################   ########################################################################################",
        "################################   ########################################################################################",
        "################################^^^########################################################################################"
    },
    // LEVEL 2: "The Vertical Climb" (Emphasizing verticality and introducing the concept of secret warps)
    {
        "                                                                                                                        ",
        "                                                                   V                                                    ",
        "                                                      XXXXX       XXX                                                   ",
        "                                                                                                                        ",
        "                                                                                                                        ",
        "                                       CCC        BXB                                                                   ",
        "                                      C   C                                                                             ",
        "                                     C     C      CHC                                                                   ",
        "                        BB    C     C      XXX   BBXBB                CC                                                ",
        "                     BBBB     X   XBX                                 BB        ?                                       ",
        "                    BB                                                                                                  ",
        "                                   CCC      C C C C                                                              F      ",
        "                  H               C   C     BBBBBBB                             H                                #      ",
        "P      E         BBB         CCC C     C            E   E            BBBB      BBB                       # E E E #      ",
        "#########      #######     #######     #######################    ###################    ###############################",
        "#########^^^^^^#######^^^^^#######^^^^^#######################^^^^###################^^^^###############################",
        "#########^^^^^^#######^^^^^#######^^^^^#######################^^^^###################^^^^###############################"
    },
    // LEVEL 3: "Subwoofer Caverns" (Claustrophobic, introducing Droppers carefully)
    {
        "########################################################################################################################",
        "########################################################################################################################",
        "##                                                                                                                    ##",
        "##                                                    D                       D                                       ##",
        "##                                                   # #                     # #                                      ##",
        "##                            D                     ## ##                   ## ##                                     ##",
        "##                          ## ##                                                                                     ##",
        "##   C C C                                                                              C C                           ##",
        "##   B B B             C                   C                      C                    B B B                          ##",
        "##                    B?B                 B B                    B B                                      X           ##",
        "##                                                                                                       XX       F   ##",
        "##P         E                     E                   E     XX       X  X E XXXXX     X  E  X      E    XXX       #   ##",
        "########   ###      #######      ###     ######     #####       #####    ###     #######################################",
        "########^^^###^^^^^^#######^^^^^^###^^^^^######^^^^^#####^^^^^^^#####^^^^###^^^^^#######################################",
        "########################################################################################################################"
    },
    // LEVEL 4: "Crystal Steps" (Relying on jumping limits)
    {
        "                                                                                                                        ",
        "##############################D#D#D#D#D#D#D#D####################################BBBBBBBB##############################W",
        "##                                                                               BB                                   ##",
        "##                                                                               BB  BBBB                             ##",
        "##                                                                               BBB BBBB                             ##",
        "##                                                                               BBB BBBB    CCCC                     ##",
        "##                      CC                                                       BBB BBBB   BBBBBB                    ##",
        "##                    CC  CC                                                     BBBBBBBB                             ##",
        "##                   C      C                         XXX                        BBBB                                 ##",
        "## ?              XXXX      XXXXXXXXXXXXXXXXXXXX      XXX                        BBBB BBB                             ##",
        "##                X##X      X##################X      XXX                        BBBB BBB                     X   F   ##",
        "##P     CC  CC                                   XX   XXX                            BBBB                     X   #   ##",
        "######  XX  XX                                      XXXXXXX    XXX E EXXX   ######################  X  X  X  X    #   ##",
        "######^^XX^^XX^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^#######^^^^XXXXXXXXXX^^^######################^^^^^^^^^^^^^^^^#^^^##",
        "########################################################################################################################"
    },
    // LEVEL 5: "Frostbite Towers" (Precision jumping)
    {
        "                                                                                             CC                         ",
        "                                                                                   CC        XX                      F  ",
        "                                                                                                                   XXXX ",
        "                                                                                   BB    ###               ####     XX  ",
        "                                                                           #####                     ####           XX  ",
        "                                                                           BDBDB                                    XX  ",
        "                                                                 BB#X# CC                     BB                    XX  ",
        "                           C C      C        C        C                 C                                           XX  ",
        "                                    D        D        D         B#X#    C                                           XX  ",
        "                  E       C   C                                         CC               #??#                       XX  ",
        "        ######  #####                                          #X#                             # EE #               XX  ",
        "P            #         ####   #####    #####    #####    ####          ####BBBBB#####    ############               XX  ",
        "########^^^^^#  H   H  #^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^XX##",
        "########################################################################################################################",
        "########################################################################################################################"
    },
    // LEVEL 6: "The Bridge of Destruction" (Falling hazards and long jumps)
    {
        "                    D                                                                                       ??          ",
        "                                             B                                                                   C      ",
        "                                            DXBB                                                                        ",
        "                    ?                        X  BBBB                                         BBX      ??                ",
        "                                             XX     BBBBBBBBBBBBB                      BBBBBB  X                        ",
        "                    C                         X                  BBBBBBBBBBBBBBBBBBBBBB       XX            XX   X    F ",
        "                    #                                                                         X             XX        X ",
        "                   ###                             CC             C                                                  XX ",
        "                                               XXX XX     CC                                          XX              X ",
        "      C  C     CC       CC          C   C     XXX   XBXX XXB  X   D           C       CC      XX                     XX ",
        "        H     ####     BBBB      H            X                 X XXBXX XX    X  XX  XXXX  XXXX                       X ",
        "P     ####         X X        ####          ###X                                             XXX                      X ",
        "#####^^^^^^####^^^^^^^^^^^^####      XX  ^^## XX                                              X                    #####",
        "##############################      ^^^^^####  X                                              XX       X EX        #####",
        "##############################^^^^^^#######   XX                                              XX       ###        XX####",
        "##########################################    XX^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^XX^^^^^^^####^^^^^^^^#####"
    },
    // LEVEL 7: "Auto-Tune Castle" (Tight spike jumps)
    {
        "  XXXXXXXXXXXXXXXXXXXXXDXXXXXXXDXXXDXXXXXXXXXXXXXXXXXXXXXXDXXXXXXXXXXXXXDXXXXXDXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
        "  XXXX    CCCCCC                    XX             XX          XXXX      X   X                              XXX    XXXXX",
        "  XXXD    BB??BB                     XX           XX             XX      XB X                                X     CXXXX",
        "  XX                                  XXXX  E   XXX               X       BX                                        XXX ",
        "  XX           E                         XXXX  XXX                XX       XX            C   CC   CC   CC   CCC     XX  ",
        "  XX     CXXXXXXC                           XXXX X                 X  CC       CC       XX   XX   XX   XX   XXX  C  XX  ",
        "  XX    CX      XC                               XX                                   XXX                    XX  X EX   ",
        "  BB    B        B                   CC       CC                  X  E    H    E   XXXXX                   B XX  XXX    ",
        "        X        X                  C  C     C  C    CC          XXXXXXXXXXXXXXXXXXX XX                      XX         ",
        "                         C   C     C    C   C    C              XX #X#   #X#   #X#                        B  XX        F",
        "   XXXXXXX      XXX      B   B    C      C C      C C  C       XX  ###   ###   ###                    B B   ^XX CC  XXXX",
        "P XXX    X^^^^^^X     X ^^^^^^^ XXX       CX       CX  X ^^^ XXX    #     #     #            ^^ CCC ^^      XXXXXX  XXX#",
        "XXXXX    XXXXXXXX^   ^X^XXXXXXX^XX        X        X   X^XXX^X      #     #     #            XXXXXXXXX      XXXX    X###",
        "##XXX^^^^XXXXXXXXX^^^XXXXXXXXXXXX^^^^^^^^^^^^^^^^^^^^^^XXXXXXX^^^^^^#^^^^^#^^^^^#^^^^^^^^^^^^XXXXXXXXX^^^^^^XXXX^^^^X###",
        "##XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX#XXXXX#XXXXX#XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX###"
    },
    // LEVEL 8: "The Final Gauntlet" (Testing all skills)
    {
        "XXXXXXXXXXXXXXXXXX  X^X                                                                                                 ",
        " X            BBXXXXXXXX                                          CC   CC                                               ",
        " XUXX                                                             B#    B                                               ",
        "  X   BXB                                                         ##    #                                               ",
        "  XX            XX          C C C                                B#     #B#                                             ",
        "  XX            XXXX       BBBBBBB                    H         ####     #BB                                            ",
        "  X       XX    XX XX     #####  ##                  ##        BB#B#     BB##                                           ",
        "  XX            XX       ##    #####                #####  #E##B##B         B#                                          ",
        "  XX          XXX        ###                 ###   ####### ###BB##          ##B#                                        ",
        "                XX     #  ## CCCCC        #######  ####### ##B  #B          BB###         XXXXXXXXXXXXXXXXXXX           ",
        "                XX    ##  #  ######  ## #########   ?????? ##^^^^^          ^^^#####     XX  X  X  X  X  X  XX         #",
        "P       XXXX    XX   ### ###          ##^^####      CCCCCC ##^^^^^^^^^      ^^^^^^###   XX  X XX XX XX XX X  XX    F ###",
        "XXX X ^^^^^XXXX X^^^^##  ###^^^^^^^^^^######^^^^   ############^^^^^^^^^^^^^^^^^^^^####XX^^X^^^^^^^^^^^^^^^X^^XX#  #####",
        "XXX X^XXXXXXX##^X#####    ######################^  #############^^^^^^^^^^^^^^^^^######XX^X^^^^^^^^^^^^^^^^^X^XX########",
        "XXX XXXXXX####X######     #######################  #############^^^^^^^^^^^^^^^^^^#####XXX^^^^^^^^^^^^^^^^^^^XXX########",
        "XXX^XXXX#############^^^^^#######################^^###########^^^^^^^^^^^^^^^^^^^^#####XX^^^^^^^^^^^^^^^^^^^^^XX########"
    },

    // ----------------------------------------------------------------------------------------------------------------------
    // SECRET LEVEL 1: "The Coin Vault" (Warps from 'W')
    {
        "########################################################################################################################",
        "####################################################D###D###D###########################################################",
        "######### ######D###########      ###########   ##X? ?X? ?X? ?X#####  ##########     ###################################",
        "#####D#       ## ###                              XC C C C C CX                                 ########################",
        "### # #        # #                                X           X                                   ######################",
        "##  # #        # #                                X           X                                          ###############",
        "##                                                X           X                                           ##############",
        "##     CC                                         X  XXX XXX  X                                           ##############",
        "##    C  C                                         CCC CCC CCC                                             #############",
        "##                  CCCCC                         XX XXX XXX XX                                                #########",
        "##             E    #####     CCCC    CCCC    CCCCXCCCCCCCCCCCC          CC     CCCC    CCCC       C          ##########",
        "##P          ###              ####    ####    ####X# ### ### #X     CCC  ##     ####    ####    CC #   F           #####",
        "##### ^^^^ ###^^^^^^^^^^^^^^^^^##^^^^^^##^^^^^^##^X^^^^#^#^^^^X#    ###  ##^^^^^^##^^^^^^##^^^^ ## #  ### ^^^^^^^^^#####",
        "#####^####^#######################################X###########X#^^^^###^^######################^##^#^^###^##############",
        "########################################################################################################################",
        "########################################################################################################################"
    },
    // SECRET LEVEL 2: "Cloud Jump" (Warps from 'V')
    {
        "                                                                                                                        ",
        "                                                                                                                        ",
        "                                                                                                                        ",
        "                                                                                                                     F  ",
        "                                                                              CCC    CC   CC   CC   CC   CC   CC   BBBB ",
        "                                                ??                           CBBBC   BB   BB   BB   BB   BB   BB    BB  ",
        "                                                                        CC   BBXBB    X   X    X     X   X     X    ##  ",
        "       ?     ?     H                CC                                 CBBC                                        #?#  ",
        "                  BB               C  C                        CCCC    BXXB                                      CC  #  ",
        "                 BXXB        C    C    C        CC            CBBBBC                                    C    BB  ##C #  ",
        "       C     C              C C  C      C       BB            BBXXBB         C                 C       CBC    B    #C#  ",
        "       B     B             C   BBB       C     BB#B            CC           CCC               CCC      BBB    #     #   ",
        "      BBB   BBB            C  BBBBB       BB     #     BBBB    CC   BBB     BBB      BBB     BBBBB      B     #      #  ",
        "P      #     #            C     #        BBBB   #     BBBBBB   BB    #     BB#BB           BBB###BBB    B     #     #   ",
        "BBB    #     # BBB      BBB     #        B##B   #       ##    BBBB   #       #                 ##       #     #     #   ",
        "BB#^^^^#^^^^^#BBBBB ^^ BBBBB ^^^#^^^^^^^^^##^^^^^#^^^^^^##^^^^^^#^^^^#^^^^^^^#^^^^^^^^^^^^^^^^^#^^^^^^^^#^^^^^#^^^^^#^^^",
        "BB#^^^^#^^^^^#^^#^^^^^^^^#^^^^^^#^^^^^^^^^##^^^^^#^^^^^^##^^^^^#^^^^^#^^^^^^^#^^^^^^^^^^^^^^^^^#^^^^^^^^#^^^^^#^^^^^#^^^"
    },
    // SECRET LEVEL 3: "Lava Stash" (Warps from 'U')
    {
        "##############D##########D##########D##########D##########D##########D##########D#########################################",
        "##                                                                                                   CCCCCCCCCC         ##",
        "##                                                                                                   B?BB??BB?B         ##",
        "##                                                                                                   CCCCCCCCCC         ##",
        "##                                                                                                   BB??BB??BB         ##",
        "##                                                                                                   CCCCCCCCCC         ##",
        "##      CC         CC         CC         CC         CC         CC         CC         CC              B?BB??BB?B         ##",
        "##     C  C       C  C       C  C       C  C       C  C       C  C       C  C       C  C             CCCCCCCCCC         ##",
        "##    C    C     C    C     C    C     C    C     C    C     C    C     C    C     C    C            BB??BB??BB   F     ##",
        "##   C      C   C      C   C      C   C      C   C      C   C      C   C      C   C      C           CCCCCCCCCC   #     ##",
        "##P  C      C   C      C   C      C   C      C   C      C   C      C   C      C   C      C  XX  XX  XXXXXXXXXXX  ##     ##",
        "######      XXXXX      XXXXX      XXXXX      XXXXX      XXXXX      XXXXX      XXXXX      XXXXXEEEEEEE##XX??XXCCC#####   ##",
        "######^^^^^^XXXXX^^^^^^XXXXX^^^^^^XXXXX^^^^^^XXXXX^^^^^^XXXXX^^^^^^XXXXX^^^^^^XXXXX^^^^^^XXXXX##########CCCCCC#######^^^##",
        "######^^^^^^XXXXX^^^^^^XXXXX^^^^^^XXXXX^^^^^^XXXXX^^^^^^XXXXX^^^^^^XXXXX^^^^^^XXXXX^^^^^^XXXXXCCCCCCCCCCCXX##########^^^##",
        "#############XXX########XXX########XXX########XXX########XXX########XXX########XXX########XXX#############################"
    }
};

const char* LEVEL_TITLES[MAX_LEVELS] = {
    "LEVEL 1: Brick Breaker Lane",
    "LEVEL 2: The Vertical Climb",
    "LEVEL 3: Subwoofer Caverns",
    "LEVEL 4: Crystal Steps",
    "LEVEL 5: Frostbite Towers",
    "LEVEL 6: Bridge of Destruction",
    "LEVEL 7: Auto-Tune Castle",
    "LEVEL 8: The Final Gauntlet",
    "SECRET 1: The Coin Vault",
    "SECRET 2: Cloud Heaven",
    "SECRET 3: The Lava Stash"
};

const char* LORE_TEXT[MAX_LEVELS] = {
    "Smash the bricks to find the path!",
    "Look up! A secret warp lies above.",
    "The caves are deep, watch out for Droppers.",
    "Careful on the crystal gaps.",
    "Snow and ice... time your jumps well.",
    "The bridge is out. Keep moving!",
    "The King's fortress. Spikes everywhere.",
    "This is it. Save the music!",
    "You found a secret pipe!",
    "You found a secret pipe!",
    "You found a secret pipe!"
};

#endif