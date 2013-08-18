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
*   File Name   : Caspar_VideoDevice.h
*   Version     : 1.0
*   Description : Caspar supports both CG and media. This plugin works with
*                 Media.
*
*****************************************************************************/


#pragma once

#include <cstring>
#include <sstream>
#include <cstdlib>
#include <queue>
#include <pthread.h>

#include "libCaspar/libCaspar.h"
#include "VideoDevice.h"
#include "PluginConfig.h"

class threadCom
{
public:
    CasparConnection *m_pcaspcon;
    std::queue<CasparCommand> *m_pcommandqueue;
    std::queue<std::vector<std::string>> *m_presponsequeue;
    pthread_mutex_t *m_pqueuemutex;
    Log *m_plogger;
    plugin_status_t *m_pstatus;
};

/**
 * Caspar supports both CG and media. This plugin works with Media.
 *
 * @param config Configuration data for this plugin
 * @param h      Link back to the GlobalStuff structures
 */
class VideoDevice_Caspar: public VideoDevice
{
public:
    VideoDevice_Caspar (PluginConfig config, Hook h);
    virtual ~VideoDevice_Caspar ();

    void updateHardwareStatus ();
    void getFiles ();
    void cue (std::string clip);
    void play ();
    void stop ();
    virtual void poll ();
private:
    std::shared_ptr<CasparConnection> m_pcaspcon;

    //! Configured hostname and port number
    std::string m_hostname;
    std::string m_port;

    void fileUpdateJob (std::shared_ptr<std::map<std::string, VideoFile>> newfiles,
            std::shared_ptr<std::vector<std::string>> deletedfiles, std::shared_ptr<void> data,
            std::timed_mutex &core_lock, std::string hostname, std::string port,
            std::shared_ptr<std::vector<std::string>> transformed_files);
    void fileUpdateComplete (std::shared_ptr<std::map<std::string, VideoFile>> newfiles,
            std::shared_ptr<std::vector<std::string>> deletedfiles, std::shared_ptr<void> data);
    void batchFileLengths (std::vector<std::string>& medialist, std::shared_ptr<CasparConnection> pccon,
            std::shared_ptr<std::map<std::string, VideoFile>> newfiles);

    void cb_updatefiles (std::vector<std::string>& resp, std::shared_ptr<CasparConnection> pccon,
            std::shared_ptr<std::map<std::string, VideoFile>> newfiles,
            std::shared_ptr<std::vector<std::string>> deletedfiles,
            std::shared_ptr<std::vector<std::string>> transformed_files);
    void cb_updatelength (std::vector<std::string>& medialist, std::vector<std::string>& resp,
            std::shared_ptr<CasparConnection> pccon, std::shared_ptr<std::map<std::string, VideoFile>> newfiles);
    void cb_info (std::vector<std::string>& resp);
};

