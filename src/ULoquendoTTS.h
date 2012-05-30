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
 * File:   UEigenfaces.h
 * Author: lmalek
 *
 * Created on 21 marzec 2012, 09:55
 */

#ifndef UEIGENFACES_H
#define	UEIGENFACES_H

#include <urbi/uobject.hh>
#include <loqtts.h>

/* singelton
 */
class LoquendoTTSEngine {
public:
    static LoquendoTTSEngine& getInstance();
    ~LoquendoTTSEngine();
    void speak(std::string text);
    void registerCallback(void* callback, void *data);
private:
    std::string voiceName;
    ttsHandleType hReader; /* Reader handle */
    LoquendoTTSEngine();
    LoquendoTTSEngine(const LoquendoTTSEngine&);
    LoquendoTTSEngine& operator=(const LoquendoTTSEngine&);
};

static void pfnCallback(size_t PromptID, ttsEventType evt, void *lData, void *pUser);

class ULoquendoTTS : public urbi::UObject {
public:
    friend void pfnCallback(size_t PromptID, ttsEventType evt, void *lData, void *pUser);
    ULoquendoTTS(const std::string& name);
    virtual ~ULoquendoTTS();

    /** 
     * Initialization function
     * Responsible for initalization
     */
    void init(int);

    void speak(std::string text);
    void setVoiceName(std::string voice);
    void setLanguage(std::string language);

    urbi::UEvent newVisemes;
    urbi::UEvent newSpeaking;

    urbi::UVar visemeId;
    urbi::UVar visemeTime;
    urbi::UVar visemeSet;
    urbi::UVar visemeTrig;
    urbi::UVar isSpeaking;

    std::vector<int> visIds;
    std::vector<double> visTimes;
    
private:

};

#endif	/* UEIGENFACES_H */

