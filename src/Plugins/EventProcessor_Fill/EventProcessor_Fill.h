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
*   File Name   : EventProcessor_Fill.cpp
*   Version     : 1.0
*   Description : Definition of an EventProcessor to generate events for
*   idents, trailers and continuity graphics to fill schedules. Also contains
*   a FillDB class to extend MemDB and provide a database backend.
*
*****************************************************************************/

#pragma once

#define INVALID_QUERY_POINTER -1

#include <map>
#include <set>

#include "MouseCatcherProcessorPlugin.h"
#include "PluginConfig.h"
#include "MemDB.h"

class FillDB : public MemDB
{
public:
    FillDB(std::string databasefile, std::map<int, int>& weightpoints,
            int fileweight);

    void updateWeightPoints (std::map<int, int>& weightpoints, int fileweight);
    void addPlay (int idnumber, int time);
    void addFile (std::string filename, std::string device, std::string type,
            int duration, int weight);
    void addPlayData (std::vector<std::pair<int,int>>& playdata);
    int getBestFile (std::string& filename, int inserttime, int duration,
    		std::string device, std::string type, int& resultduration,
    		std::set<int>& excludeid);
    void syncDatabase(std::string databasefile);


private:
    DBQuery* m_pgetbestfile_query;
    DBQuery* m_paddplay_query;
    DBQuery* m_paddfile_query;
};

/**
 * EventProcessor to automatically fill time with idents, trailers and
 * continuity blocks.
 */
class EventProcessor_Fill : public MouseCatcherProcessorPlugin {
public:
    EventProcessor_Fill(PluginConfig config, Hook h);
    void readConfig(PluginConfig config);
    void handleEvent(MouseCatcherEvent originalEvent,
            MouseCatcherEvent& resultingEvent);
    void addFile (std::string filename, std::string device, std::string type,
            int duration, int weight);

    std::shared_ptr<FillDB> m_pdb;
private:
    std::string m_dbfile;
    std::map<int, int> m_weightpoints;
    int m_fileweight;

    std::vector<std::pair<std::string, std::string>> m_structuredata; ///< An example could be {"ident", "device1"}
    bool m_filler; ///< Whether to fill remaining time with the last item.

    MouseCatcherEvent m_continuityfill; ///< Event to tack on the end to fill remaining time
    int m_continuitymin; ///< Minimum length for continuity fill
};