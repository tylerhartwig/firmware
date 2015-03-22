/*
 * Copyright 2015 BrewPi/Elco Jacobs/Matthew McGowan.
 *
 * This file is part of BrewPi.
 * 
 * BrewPi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * BrewPi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with BrewPi.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Brewpi.h"
#include "fixstl.h"
#include "UI.h"
#include "Buzzer.h"
#include "eGuiSettings.h"
#include "ConnectedDevicesManager.h"
#include "PiLink.h"

#include "devicetest/device_test_screen.h"
#include "controller/controller_screen.h"

extern "C" {
#include "d4d.h"
}

eGuiSettingsClass eGuiSettings;

uint8_t UI::init() {
    if (!D4D_Init(NULL))
        return 1;
    
    D4D_SetOrientation(D4D_ORIENT_LANDSCAPE);
    #if BREWPI_BUZZER
	buzzer.init();
	buzzer.beep(2, 100);
    #endif
        
    return 0;
}

uint32_t UI::showStartupPage()
{
    // Check if touch screen has been calibrated
    if (!eGuiSettings.loadTouchCalib()) {
        // could not load valid settings from flash memory
        calibrateTouchScreen();
    }
            
    D4D_ActivateScreen(&screen_controller, D4D_TRUE);
    D4D_Poll();
    return 0;
}

/**
 * Show the main controller page. 
 */
void UI::showControllerPage() {
    // for now we in fact show what will be the startup page.     
}

void UI::ticks()
{
    D4D_TimeTickPut();
    D4D_CheckTouchScreen();
    D4D_Poll();
    
    #if BREWPI_BUZZER
	buzzer.setActive(alarm.isActive() && !buzzer.isActive());
    #endif    
}    

void UI::update() 
{   
    // todo - how to forward the update to the right screen

}

/**
 * Show touch screen calibration screen store settings afterwards
 */
void UI::calibrateTouchScreen() {
    D4D_CalibrateTouchScreen();
    eGuiSettings.storeTouchCalib();
}