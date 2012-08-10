/* Libvisual - The audio visualisation framework cli tool
 *
 * Copyright (C) 2012 Libvisual team
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef _LV_TOOL_DISPLAY_DRIVER_FACTORY_HPP
#define _LV_TOOL_DISPLAY_DRIVER_FACTORY_HPP

#include "display_driver.hpp"
#include <memory>
#include <functional>
#include <vector>

typedef std::function<DisplayDriver* (Display& display)> DisplayDriverCreator;

typedef std::vector<std::string> DisplayDriverList;

class DisplayDriverFactory
{
public:

    typedef DisplayDriverCreator Creator;

    static DisplayDriverFactory& instance ()
    {
	    static DisplayDriverFactory m_instance;
	    return m_instance;
    }

    DisplayDriver* make (std::string const& name, Display& display);

    void add_driver (std::string const& name, Creator const& creator);

    bool has_driver (std::string const& name) const;

    DisplayDriverList get_driver_list () const;

private:

    class Impl;
    const std::unique_ptr<Impl> m_impl;

    DisplayDriverFactory ();

    ~DisplayDriverFactory ();
};

#endif // _LV_TOOL_DISPLAY_DRIVER_FACTORY_HPP
