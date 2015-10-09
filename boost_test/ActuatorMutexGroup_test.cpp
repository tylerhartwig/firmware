/*
* Copyright 2015 BrewPi/Elco Jacobs.
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

#include <boost/test/unit_test.hpp>

#include "runner.h"

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time, to seed rand */
#include <cstring>

#include "ActuatorInterfaces.h"
#include "ActuatorTimeLimited.h"
#include "ActuatorMocks.h"
#include "Ticks.h"
#include "ActuatorMutexGroup.h"
#include "ActuatorMutexDriver.h"

BOOST_AUTO_TEST_SUITE(ActuatorMutexTest)

BOOST_AUTO_TEST_CASE(two_actuators_belonging_to_the_same_group_cannot_be_active_at_once) {
    ActuatorDigital * act1 = new ActuatorBool();
    ActuatorDigital * act2 = new ActuatorBool();
    ActuatorMutexGroup * mutex = new ActuatorMutexGroup();

    ActuatorMutexDriver * actm1 = new ActuatorMutexDriver(act1);
    ActuatorMutexDriver * actm2 = new ActuatorMutexDriver(act2);
    actm1->setMutex(mutex);
    actm2->setMutex(mutex);

    actm1->setActive(true, 5);
    BOOST_CHECK(actm1->isActive()); // actuator is first, so is allowed to go active
    BOOST_CHECK(act1->isActive()); // target actuator is active

    actm2->setActive(true, 10);
    BOOST_CHECK(!act2->isActive()); // actuator 2 is not allowed to go active while act1 is still active

    actm1->setActive(false, 5);
    BOOST_CHECK(!act1->isActive());
    actm1->setActive(true, 5);
    BOOST_CHECK(!act1->isActive()); // second request to go active from act1 is blocked, because act2 has an open request with higher priority

    actm2->setActive(true, 10);
    BOOST_CHECK(act2->isActive()); // actuator 2 is now allowed to go active
}


BOOST_AUTO_TEST_CASE(dead_time_between_actuators_is_honored) {
    ActuatorDigital * act1 = new ActuatorBool();
    ActuatorDigital * act2 = new ActuatorBool();
    ActuatorMutexGroup * mutex = new ActuatorMutexGroup();

    ActuatorMutexDriver * actm1 = new ActuatorMutexDriver(act1);
    ActuatorMutexDriver * actm2 = new ActuatorMutexDriver(act2);
    actm1->setMutex(mutex);
    actm2->setMutex(mutex);

    mutex->setDeadTime(10000); // 10 seconds dead time

    actm1->setActive(true, 5);
    BOOST_CHECK(actm1->isActive()); // actuator is first, so is allowed to go active
    BOOST_CHECK(act1->isActive()); // target actuator is active

    delay(1000);

    actm2->setActive(true, 10);
    BOOST_CHECK(!act2->isActive()); // actuator 2 is not allowed to go active while act1 is still active

    actm1->setActive(false, 5);
    BOOST_CHECK(!actm1->isActive());
    actm1->setActive(true, 5);
    BOOST_CHECK(!actm1->isActive()); // second request to go active from act1 is blocked, because act2 has an open request with higher priority

    actm2->setActive(true, 10);
    BOOST_CHECK(!actm2->isActive()); // actuator 2 is still not allowed to go active, because of dead time

    int i;
    for(i = 1; i<20; i++){
        delay(1000);
        actm2->setActive(true, 10);
        if(actm2->isActive()){
            break;
        }

    }
    BOOST_CHECK(actm2->isActive());
    BOOST_CHECK_EQUAL(i,10); // act2 was allowed to go active after 10 seconds
}

BOOST_AUTO_TEST_SUITE_END()
