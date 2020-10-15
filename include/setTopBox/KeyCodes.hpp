#pragma once

#include <string>
#include <vector>

namespace setTopBox
{
    struct Key{
        std::string keyName;
        std::string keyValue;
        std::string description;
    };

    static std::vector<Key> keyCodes = { {"KEY_IP_POWER", "0x0100", "Power on/off" },
                                {"KEY_MUTE", "0x0105", "This key is used to set the mute mode." },
                                {"KEY_1", "0x0031" , "1" },
                                {"KEY_2", "0x0032" , "2" },
                                {"KEY_3", "0x0033" , "3" },
                                {"KEY_4", "0x0034" , "4" },
                                {"KEY_5", "0x0035" , "5" },
                                {"KEY_6", "0x0036" , "6" },
                                {"KEY_7", "0x0037" , "7" },
                                {"KEY_8", "0x0038" , "8" },
                                {"KEY_9", "0x0039" , "9" },
                                {"KEY_FAVORITE", "0x0119", "This key is used to add a program to favorites." },
                                {"KEY_0", "0x0030" , "0" },
                                {"KEY_DVB_TXT", "0x010D" , "This key is used to open or close teletext messages on the current channel." },
                                {"KEY_MANU", "0x0110" , "Main manu" },
                                {"KEY_EPG", "0x0111" , "Program Guidelines" },
                                {"KEY_PROGRAM_INFO", "0x010C" , "Jumps to program information page." },
                                {"KEY_VOL_UP", "0x0103" , "Vol+, this key is used to increase volume." },
                                {"KEY_VOL_DOWN", "0x0104" , "Vol-, this key is used to decrease volume." },
                                {"KEY_CHANNEL_UP", "0x0101" , "ch+, this key is used to increase channels. " },
                                {"KEY_CHANNEL_DOWN", "0x0102" , "ch--, this key is used to decrease channels." },
                                {"KEY_LEFT", "0x0025" , "This is the LEFT arrow key." },
                                {"KEY_RIGHT", "0x0027" , "This is the RIGHT arrow key." },
                                {"KEY_UP", "0x0026" , "This is the UP arrow key." },
                                {"KEY_DOWN", "0x0028" , "This is the DOWN arrow key." },
                                {"KEY_ENTER", "0x000D" , "This key is used to confirm an operation." },
                                {"KEY_BACK", "0x0069" , "Return to previous menu." },
                                {"KEY_RECORDINGS", "0x045F" , "This is the shortcut key to access program recording services." },
                                {"KEY_VOD", "0x0018" , "This is the VOD shortcut key." },
                                {"KEY_RED", "0x0113" , "This is a shortcut key(in red)." },
                                {"KEY_GREEN", "0x0114" , "This is a shortcut key(in green)." },
                                {"KEY_YELLOW", "0x0115" , "This is a shortcut key(in yellow)." },
                                {"KEY_BLUE", "0x0116" , "This is a shortcut key(in blue)." },
                                {"KEY_FAST_REWIND", "0x0109" , "This key is used to rewind a program." },
                                {"KEY_PLAY", "0x045A" , "This key is used to play or pause a program." },
                                {"KEY_FAST_FORWARD", "0x0108" , "This key is used to fast forward a program." },
                                {"KEY_STOP", "0x010E" , "This key is used to pause program or stop recording." },
                                {"KEY_PAUSE", "0x0107" , "This key is used to pause a program." },
                                {"KEY_REC_ACTIVE", "0x045C" , "This key is used to start the recording of the current program. The key icon is in red." },
                                {"KEY_TV", "0x0454" , "This is the TV Guide shortcut key." },
                                {"KEY_RADIO", "0x0458" , "This is the RADIO Guide shortcut key." },
                                {"KEY_MAIL", "0x0459" , "This is the BMAIL shortcut key." },
                                {"KEY_HELP", "0x011C" , "Help" },
                                {"KEY_SEARCH", "0x0451" , "This key is used to search programs." },
                                {"KEY_SIGNAL_CHECK", "0x0083" , "Check signal reception" } };
}