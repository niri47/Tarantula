/******************************************************************************
*   Copyright (C) 2011 - 2013  York Student Television
*
*   Tarantula is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   Tarantula is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with Tarantula.  If not, see <http://www.gnu.org/licenses/>.
*
*   Contact     : tarantula@ystv.co.uk
*
*   File Name   : Channel.cpp
*   Version     : 1.0
*   Description : Class for running channels and all event handling
*
*****************************************************************************/


#include "Channel.h"
#include "CrosspointDevice.h"
#include "VideoDevice.h"
#include "CGDevice.h"
#include "Misc.h"

//this is here because if it were in core header, it would create a loop.
extern std::vector<Channel*> g_channels;

int end = 0;

/**
 * Constructor when a name isn't explicitly specified
 */
Channel::Channel ()
{
    m_channame = "Unnamed Channel";
    m_xpport = "YSTV Stream";
    m_xpdevicename = "DemoXpointDefaultName";
    //try and get a more sensible default XP name
    for (std::pair<std::string, Device*> currentdevice : g_devices)
    {
        if (currentdevice.second->getType() == EVENTDEVICE_CROSSPOINT)
        {
            m_xpdevicename = currentdevice.first;
        }
    }
    init();
}

/**
 * Constructor specifying some more channel details
 *
 * @param name   The name of the channel to initialize
 * @param xpname The name of the crosspoint for this channel
 * @param xport  The name of this channel's crosspoint port (as in crosspoint device file)
 */
Channel::Channel (std::string name, std::string xpname, std::string xport)
{
    m_channame = name;
    m_xpdevicename = xpname;
    m_xpport = xport;
    init();
}

Channel::~Channel ()
{

}

/**
 * Check that this channel's crosspoint details exist, called after device loading
 */
void Channel::init ()
{
    // Confirm this crosspoint device actually exists
    if (g_devices.find(m_xpdevicename) == g_devices.end())
    {
        g_logger.error("Channel " + m_channame,
                "Crosspoint device " + m_xpdevicename + " does not exist!!");

        throw (std::exception());
    }

    // And check the port is real too
    if ((static_cast<CrosspointDevice*> (g_devices[m_xpdevicename]))->
            getVideoPortfromName(m_xpport) < 0)
    {
        g_logger.error("Channel " + m_channame,
                "Crosspoint port " + m_xpport + " does not exist!!");

        throw (std::exception());
    }

}

/**
 * Queries events to take place this frame and executes them
 */
void Channel::tick ()
{
    //Pull all the time triggered events at the current time
    std::vector<PlaylistEntry> events = m_pl.getEvents(EVENT_FIXED,
            static_cast<long long int> (time(NULL)));

    //Execute events on devices
    for (PlaylistEntry thisevent : events)
    {
        runEvent(&thisevent);
    }
}

/**
 * Processes child events from an event which has started
 *
 *  @param name The name of the device firing the callback
 *  @param id   The id of the event which has started playing
 */
void Channel::begunPlaying (std::string name, int id)
{
    std::vector<PlaylistEntry> childevents = m_pl.getChildEvents(id);
    // Run the child events
    for (PlaylistEntry thisevent : childevents)
    {
        runEvent(&thisevent);
    }
}

/**
 * Doesn't really do anything since relative events were scrapped
 *
 *  @param name  The name of the device firing the callback
 *  @param id    The id of the event which has stopped playing
 */
void Channel::endPlaying (std::string name, int id)
{

}

/**
 * Runs a specified event.
 *
 * @param event The event to run
 */
void Channel::runEvent (PlaylistEntry *pevent)
{
    if ((0 == g_devices.count(pevent->m_device))
            && (pevent->m_devicetype != EVENTDEVICE_PROCESSOR))
    {
        g_logger.warn("Channel Runner",
                "Device " + pevent->m_device + " not found for event ID "
                        + ConvertType::intToString(pevent->m_eventid));
        return;
    }

    switch (pevent->m_devicetype)
    {
        case EVENTDEVICE_CROSSPOINT:
        {
            CrosspointDevice::RunDeviceEvent(g_devices[pevent->m_device], pevent);
            break;
        }
        case EVENTDEVICE_VIDEODEVICE:
        {
            VideoDevice::runDeviceEvent(g_devices[pevent->m_device], pevent);
            break;
        }
        case EVENTDEVICE_CGDEVICE:
        {
            CGDevice::runDeviceEvent(g_devices[pevent->m_device], pevent);
            break;
        }
        case EVENTDEVICE_PROCESSOR:
            break;
    }

    // Marks event as processed
    m_pl.processEvent(pevent->m_eventid);
}

int Channel::createEvent (PlaylistEntry *pev)
{
    int ret = m_pl.addEvent(pev);
    return ret;
}

/**
 *  Because you cannot add member function pointers to the callbacks, here is a workaround:
 *  We go through and call the tick function of each Channel class in the host's stack.
 */
void channelTick ()
{
    for (Channel* pthischannel : g_channels)
    {
        pthischannel->tick();
    }
}

/**
 *  Because you cannot add member function pointers to the callbacks, here is a workaround:
 *  We go through and call the BegunPlaying function of each Channel class in the host's stack.
 *
 *  @param name string The name of the device firing the callback
 *  @param id int The id of the event which has started playing
 */
void channelBegunPlaying (std::string name, int id)
{
    for (Channel* pthischannel : g_channels)
    {
        pthischannel->begunPlaying(name, id);
    }
}

/**
 *  Because you cannot add member function pointers to the callbacks, here is a workaround:
 *  We go through and call the EndPlaying function of each Channel class in the host's stack.
 *
 *  @param name string The name of the device firing the callback
 *  @param id int The id of the event which has started playing
 */
void channelEndPlaying (std::string name, int id)
{
    for (Channel* pthischannel : g_channels)
    {
        pthischannel->endPlaying(name, id);
    }
}

/**
 * Loop over the channel list and return the index of a channel from its name.
 *
 * @param channelname Name of the channel to find
 *
 * @return Index of the located channel
 */
int Channel::getChannelByName (std::string channelname)
{
    for (std::vector<Channel*>::iterator it = g_channels.begin();
            it != g_channels.end(); ++it)
    {
        if (!channelname.compare((*it)->m_channame))
        {
            return it - g_channels.begin();
        }
    }

    // At this point no channel was found with that name
    throw std::exception();
    return -1;
}

