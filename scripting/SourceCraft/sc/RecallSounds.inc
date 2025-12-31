/**
 * vim: set ai et ts=4 sw=4 syntax=sourcepawn :
 * File: RecallSounds.inc
 * Description: Define SourceCRaft Recall Sounds
 * Author(s): -=|JFH|=-Naris
 */
 
#if defined _RecallSounds_included
 #endinput
#endif
#define _RecallSounds_included

#include <lib/ResourceManager>

#include "sc/sounds"

stock const char recallSrcWav[] = "sc/pshbld00.wav";
stock const char recallQryWav[] = "sc/pshbld01.wav";
stock const char recallDstWav[] = "sc/pshbld03.wav";
stock const char recallErrWav[] = "sc/pprerr00.wav";

stock SetupRecallSounds(bool force=false, bool precache=false, bool preload=false)
{
    SetupErrorSound();
    SetupDeniedSound();
    SetupButtonSound();

    SetupSound(recallSrcWav, force, DOWNLOAD, precache, preload);
    SetupSound(recallQryWav, force, DOWNLOAD, precache, preload);
    SetupSound(recallDstWav, force, DOWNLOAD, precache, preload);
    SetupSound(recallErrWav, force, DOWNLOAD, precache, preload);
}
