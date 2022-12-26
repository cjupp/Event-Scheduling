/*--------------------------------------------------------------------------------------------------------------------
| events_scheduling.cpp: A series of objects and structures to be used with a real time clock. Creates, checks, and 
| executes scheduled events
|
| Created by:     Cameron Jupp
| Date Started:   December 23, 2022
--------------------------------------------------------------------------------------------------------------------*/ 
#include "events_scheduling.h"
#include "Arduino.h"


// -------------------------------------------------------------------------------------------------------------------- //
// ----------------------------- / ___/  / /     / _\      / ____/  / ____/  / ___/  / ____/ -------------------------- //
// ---------------------------- / /__   / /__   / /_\\     \__ \    \__ \   / __/    \__ \  --------------------------- //
// --------------------------- /____/  /____/  /_/   \\  /_____/  /_____/  /____/  /_____/ ---------------------------- //
// -------------------------------------------------------------------------------------------------------------------- //

// -------------------------------------------------------------------------------------------------------------------- //
// ------------------------------------------------- EVENT CLASS ------------------------------------------------------ //
// -------------------------------------------------------------------------------------------------------------------- //


/*---------------------------------------------------------------------------------------------------------------------
| {FUNCTION NAME}: 
|        -------------------------------------------------------------------------------------------------------
| Arguments:
| - 
|        -------------------------------------------------------------------------------------------------------
| Returns: nothing
|
|        -------------------------------------------------------------------------------------------------------
| Notes/Future Changes:
| - 
|        -------------------------------------------------------------------------------------------------------
| Created by: Cameron Jupp
| Date:       {Date}
| Edited:     {Date}  
----------------------------------------------------------------------------------------------------------------------*/
void event::config(char * namePtr, clkTime time, clkTime endTime, char weekdays, void (*functPtr) (char))
{
    strcpy(this->name, namePtr);

    this->startTime = time;

    this->endTime = endTime;

    this->weekDays = weekdays;

    this->action = functPtr;

    this->configd = 1;
}


/*---------------------------------------------------------------------------------------------------------------------
| {FUNCTION NAME}: 
|        -------------------------------------------------------------------------------------------------------
| Arguments:
| - 
|        -------------------------------------------------------------------------------------------------------
| Returns: nothing
|
|        -------------------------------------------------------------------------------------------------------
| Notes/Future Changes:
| - 
|        -------------------------------------------------------------------------------------------------------
| Created by: Cameron Jupp
| Date:       {Date}
| Edited:     {Date}  
----------------------------------------------------------------------------------------------------------------------*/
void event::del()
{
    this->configd = 0;
    this->name[0] = '\0';
    this->startTime = {0,0};
    this->endTime = {0,0};
    this->weekDays = 0;
    this->action = NULL;
}


/*---------------------------------------------------------------------------------------------------------------------
| {FUNCTION NAME}: 
|        -------------------------------------------------------------------------------------------------------
| Arguments:
| - 
|        -------------------------------------------------------------------------------------------------------
| Returns: nothing
|
|        -------------------------------------------------------------------------------------------------------
| Notes/Future Changes:
| - 
|        -------------------------------------------------------------------------------------------------------
| Created by: Cameron Jupp
| Date:       {Date}
| Edited:     {Date}  
----------------------------------------------------------------------------------------------------------------------*/
void event::activate()
{
    this->active = 1;
    this->action(1);
}


/*---------------------------------------------------------------------------------------------------------------------
| {FUNCTION NAME}: 
|        -------------------------------------------------------------------------------------------------------
| Arguments:
| - 
|        -------------------------------------------------------------------------------------------------------
| Returns: nothing
|
|        -------------------------------------------------------------------------------------------------------
| Notes/Future Changes:
| - 
|        -------------------------------------------------------------------------------------------------------
| Created by: Cameron Jupp
| Date:       {Date}
| Edited:     {Date}  
----------------------------------------------------------------------------------------------------------------------*/
void event::deactivate()
{
    this->active = 0;
    this->action(0);
}


// -------------------------------------------------------------------------------------------------------------------- //
// ---------------------------------------------- EVENTHANDLER CLASS -------------------------------------------------- //
// -------------------------------------------------------------------------------------------------------------------- //


/*---------------------------------------------------------------------------------------------------------------------
| {FUNCTION NAME}: 
|        -------------------------------------------------------------------------------------------------------
| Arguments:
| - 
|        -------------------------------------------------------------------------------------------------------
| Returns: nothing
|
|        -------------------------------------------------------------------------------------------------------
| Notes/Future Changes:
| - 
|        -------------------------------------------------------------------------------------------------------
| Created by: Cameron Jupp
| Date:       {Date}
| Edited:     {Date}  
----------------------------------------------------------------------------------------------------------------------*/
char eventHandler::addEvent(char * name, clkTime time, clkTime endTime, char weekdays, void (*functPtr) (char))
{
    // If all of the events have been configured already
    if(this->numConfigd == MAX_EVENTS)
    {
    return -1;
    }
    // Otherwise, configure the next one
    else
    {
    this->events[numConfigd++].config(name, time, endTime, weekdays, functPtr);
    return 0;
    }
}

/*---------------------------------------------------------------------------------------------------------------------
| {FUNCTION NAME}: 
|        -------------------------------------------------------------------------------------------------------
| Arguments:
| - 
|        -------------------------------------------------------------------------------------------------------
| Returns: nothing
|
|        -------------------------------------------------------------------------------------------------------
| Notes/Future Changes:
| - 
|        -------------------------------------------------------------------------------------------------------
| Created by: Cameron Jupp
| Date:       {Date}
| Edited:     {Date}  
----------------------------------------------------------------------------------------------------------------------*/
// Remove an already configured event
char eventHandler::deleteEvent(char delIndex)
{
    // If there is an event to delete
    if(numConfigd)
    {
    // If the event to be deleted is in the middle of the array
    if(delIndex < numConfigd-1)
    {
        // Shift the whole array forward to fill in the gap
        this->orderEvents(delIndex);
        

        // Decriment the number configd
        this->numConfigd--;
        
    }
    // Else if the event to be deleted is the last one in the array
    else
    {
        // Set the configd bit to 0 and decriment the number configd
        this->events[delIndex].del();
        this->numConfigd--;
    }

    }
    else
    {
    // No events to delete, return error
    return -1;
    }

}

/*---------------------------------------------------------------------------------------------------------------------
| {FUNCTION NAME}: 
|        -------------------------------------------------------------------------------------------------------
| Arguments:
| - 
|        -------------------------------------------------------------------------------------------------------
| Returns: nothing
|
|        -------------------------------------------------------------------------------------------------------
| Notes/Future Changes:
| - 
|        -------------------------------------------------------------------------------------------------------
| Created by: Cameron Jupp
| Date:       {Date}
| Edited:     {Date}  
----------------------------------------------------------------------------------------------------------------------*/
// After an event is deleted, shift everything so that it is continuous and predictable
void eventHandler::orderEvents(char delIndex)
{
    // For every event after the deleted one and up to the last one configured
    for(int i = delIndex; i < numConfigd - 1; i++)
    {
    // The current event is overwritten by the next one
    this->events[i] = this->events[i+1];
    }
    // When the last configured event is reached, mark as unconfigd (delete)
    this->events[numConfigd-1].del();
}

/*---------------------------------------------------------------------------------------------------------------------
| {FUNCTION NAME}: 
|        -------------------------------------------------------------------------------------------------------
| Arguments:
| - 
|        -------------------------------------------------------------------------------------------------------
| Returns: nothing
|
|        -------------------------------------------------------------------------------------------------------
| Notes/Future Changes:
| - 
|        -------------------------------------------------------------------------------------------------------
| Created by: Cameron Jupp
| Date:       {Date}
| Edited:     {Date}  
----------------------------------------------------------------------------------------------------------------------*/
// Check for events activating
void eventHandler::check(char currHour, char currMin, char currDay)
{
    for (int i = 0; i < this->numConfigd; i++)
    {
    // If the passed in hour and minute time match, as well as the weekday, activate the event
    if(this->events[i].startTime.hr == currHour && this->events[i].startTime.min ==currMin && this->events[i].weekDays & currDay)
    {
        if(!this->events[i].active)
        {
            this->events[i].activate();
        }
    }
    // If the start time of the event plus the duration equals the passed in time, deactivate the event
    // The day of the week does not matter, as the event could be active until the next day possibly
    if(this->events[i].endTime.hr == currHour && this->events[i].endTime.min ==currMin)
    {
        if(this->events[i].active)
        {
             this->events[i].deactivate();
        }
    }
    }
}

// -------------------------------------------------------------------------------------------------------------------- //
// -------------------  / ___/  / / //  / \  / /  / ___/ /__  __/  /_  _/  / __  /  / \  / /  / ____/ ----------------- //
// ------------------  / __/   / /_//  / /\\/ /  / /__     / /      / /   / /_/ /  / /\\/ /   \__ \ ------------------- //
// -----------------  /_/     /____/  /_/  \_/  /____/    /_/     /___/  /_____/  /_/  \_/  /_____/ ------------------- //
// -------------------------------------------------------------------------------------------------------------------- //

/*---------------------------------------------------------------------------------------------------------------------
| {FUNCTION NAME}: 
|        -------------------------------------------------------------------------------------------------------
| Arguments:
| - 
|        -------------------------------------------------------------------------------------------------------
| Returns: nothing
|
|        -------------------------------------------------------------------------------------------------------
| Notes/Future Changes:
| - 
|        -------------------------------------------------------------------------------------------------------
| Created by: Cameron Jupp
| Date:       {Date}
| Edited:     {Date}  
----------------------------------------------------------------------------------------------------------------------*/
void printEvent(event * eventPtr)
{
  Serial.println("\nName: ");
  Serial.println(eventPtr->name);

  Serial.println("Configd: ");
  Serial.println(eventPtr->configd, DEC);  

  Serial.println("Start Time: ");
  Serial.println(eventPtr->startTime.hr, DEC);
  Serial.println(eventPtr->startTime.min, DEC);

  Serial.println("End Time");
  Serial.println(eventPtr->endTime.hr, DEC);
  Serial.println(eventPtr->endTime.min, DEC);

  Serial.println("Weekdays");
  Serial.println(eventPtr->weekDays, BIN);

  Serial.println();
}

