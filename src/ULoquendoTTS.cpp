/*
 * Kinect module based on OpenNI for URBI
 * Copyright (C) 2012  Lukasz Malek
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * File:   ULoquendoTTS.cpp
 * Author: lmalek
 * 
 * Created on 21 marzec 2012, 09:55
 */

#include "ULoquendoTTS.h"
#include <iostream>


using namespace std;
using namespace urbi;
using namespace boost;

static int XSAMPA2Viseme(const char* label) {
    char work = 1;
    char sign;
    while (work) {
        if (label == NULL)
            return -1;
        if (*label == '.')
            return 0;
        if (isalpha(*label)) {
            sign = tolower(*label);
            switch (sign) {
                case 'a': return 1;
                case 'b': return 21;
                case 'c': return 15;
                case 'd': return 19;
                case 'e': return 4;
                case 'f': return 18;
                case 'g': return 20;
                case 'h': return 12;
                case 'i': return 6;
                case 'j': return 16;
                case 'k': return 20;
                case 'l': return 14;
                case 'm': return 21;
                case 'n': return 19;
                case 'o': return 10;
                case 'p': return 21;
                case 'q': return 20;
                case 'r': return 13;
                case 's': return 15;
                case 't': return 17;
                case 'u': return 4;
                case 'w': return 7;
                case 'x': return 1;
                case 'y': return 6;
                case 'z': return 6;
                default:
                    return 2;
            }
        }
        label++;
    }
}

LoquendoTTSEngine& LoquendoTTSEngine::getInstance() {
    static LoquendoTTSEngine engine;
    return engine;
}

LoquendoTTSEngine::~LoquendoTTSEngine() {
    ttsDeleteSession(NULL); /* deallocates all tts resources  */
}

void LoquendoTTSEngine::speak(std::string text) {
    ttsResultType r; /* Error code returned by TTS APIs */
    
    /* Converts text to speech */
    r = ttsRead(
            hReader, /* Reader handle */
            text.c_str(),
            //	"\\demosentence", /* Input */
            ttsTRUE, /* bAsync */
            ttsFALSE, /* bFromFile */
            NULL); /* ttsRead keeps control until the end */
    if (r != tts_OK) {
        fprintf(stderr, "%s\n", ttsGetErrorMessage(r));
        throw std::runtime_error("[ULoquendoTTS]::speak() : Failed to synthetize text");
    }
}

LoquendoTTSEngine::LoquendoTTSEngine() {
    voiceName = "Dave";
    ttsResultType r; /* Error code returned by TTS APIs */
    /* Initializes a LoquendoTTS Reader using the implicit session */
    r = ttsNewReader(&hReader, NULL);
    if (r != tts_OK) {
        fprintf(stderr, "%s\n", ttsGetErrorMessage(r));
        ttsDeleteSession(NULL); /* deallocates all tts resources  */
        throw std::runtime_error("[ULoquendoTTS]::init() : Failed to initialize reader");
    }

    /* Sets the persona parameters using mother tongue and default style */
    r = ttsLoadPersona(hReader, voiceName.c_str(), NULL, NULL);
    if (r != tts_OK) {
        fprintf(stderr, "%s\n", ttsGetErrorMessage(r));
        ttsDeleteSession(NULL); /* deallocates all tts resources */
        throw std::runtime_error("[ULoquendoTTS]::init() : Failed to load voice");
    }

    /* Sends samples directly to the audio board */
    r = ttsSetAudio(hReader, "LTTS7AudioBoard", NULL, 44100, tts_LINEAR, tts_STEREO, 0);
    if (r != tts_OK) {
        fprintf(stderr, "%s\n", ttsGetErrorMessage(r));
        ttsDeleteSession(NULL); /* deallocates all tts resources  */
        throw std::runtime_error("[ULoquendoTTS]::init() : Failed to set audio");
    }
    ttsSetParam(hReader, "PhoneticCoding", "XSAMPA");
    //    r = ttsSetCallback(hReader, reinterpret_cast<void*> (&ULoquendoTTS::pfnCallback), NULL, TTSCALLBACKFUNCTION);
    //    if (r != tts_OK)
    //        throw std::runtime_error("[ULoquendoTTS]::init() : Failed to set callback");

    ttsSetTextEncoding(hReader, TTSUTF8);
    ttsSetTextFormat(hReader, TTSPLAINTEXT);
}

void LoquendoTTSEngine::registerCallback(void* callback, void* data) {
    ttsResultType r; /* Error code returned by TTS APIs */
    r = ttsSetCallback(hReader, callback, data, TTSCALLBACKFUNCTION);
    if (r != tts_OK)
        throw std::runtime_error("[ULoquendoTTS]::init() : Failed to set callback");
}

LoquendoTTSEngine::LoquendoTTSEngine(const LoquendoTTSEngine&) {
}

LoquendoTTSEngine& LoquendoTTSEngine::operator=(const LoquendoTTSEngine&) {
}

ULoquendoTTS::ULoquendoTTS(const std::string& name) : UObject(name) {
    cerr << "[ULoquendoTTS]::ULoquendoTTS()" << endl;
    UBindFunction(ULoquendoTTS, init);
}

ULoquendoTTS::~ULoquendoTTS() {
    cerr << "[ULoquendoTTS]::~ULoquendoTTS()" << endl;
}

void ULoquendoTTS::init(int) {
    cerr << "[ULoquendoTTS]::init()" << endl;

    //UBindThreadedFunction(ULoquendoTTS, speak, LOCK_FUNCTION_DROP);
    UBindFunction(ULoquendoTTS, speak);

    UBindEvent(ULoquendoTTS, newVisemes);
    UBindEvent(ULoquendoTTS, newSpeaking);

    UBindVar(ULoquendoTTS, visemeId);
    UBindVar(ULoquendoTTS, visemeTime);
    UBindVar(ULoquendoTTS, visemeSet);
    UBindVar(ULoquendoTTS, visemeTrig);
    UBindVar(ULoquendoTTS, isSpeaking);

    LoquendoTTSEngine::getInstance().registerCallback(reinterpret_cast<void*> (pfnCallback), this);
}

static void pfnCallback(size_t PromptID, ttsEventType evt, void *lData, void *pUser) {
    const char *label;
    ULoquendoTTS* loq = reinterpret_cast<ULoquendoTTS*>(pUser);
    unsigned int duration;
    ttsResultType r = tts_OK;
    switch (evt) {
        case TTSEVT_VOICEPHONEME:
            r = ttsGetPhonemeInfo(lData, &label, &duration);
            if (r == tts_OK) {
                loq->visIds.push_back(XSAMPA2Viseme(label));
                loq->visTimes.push_back(0.001 * duration);
            }
            break;
        case TTSEVT_SENTENCE:
        {
            urbi::UList payload;
            std::vector<int>::iterator visIdsIt;
            std::vector<double>::iterator visTimesIt;
            for (visIdsIt = loq->visIds.begin(), visTimesIt = loq->visTimes.begin();
                    visIdsIt != loq->visIds.end() && visTimesIt != loq->visTimes.end();
                    visIdsIt++, visTimesIt++) {
                urbi::UList visemePair;
                visemePair.push_back(*visIdsIt);
                visemePair.push_back(*visTimesIt);
                payload.push_back(visemePair);
            }
            loq->visemeId = loq->visIds;
            loq->visemeTime = loq->visTimes;
            loq->visemeSet = payload;
            loq->newVisemes.emit(payload);
            loq->visemeTrig = 1;
        }
            break;
        case TTSEVT_AUDIOSTART:
            loq->isSpeaking = 1;
            loq->newSpeaking.emit();
            break;
        case TTSEVT_ENDOFSPEECH:
            loq->isSpeaking = 0;
            break;
        default:
            // in this example we ignore the remaining events
            break;
    }
}

void ULoquendoTTS::speak(std::string text) {
    LoquendoTTSEngine::getInstance().speak(text);
}

UStart(ULoquendoTTS);
