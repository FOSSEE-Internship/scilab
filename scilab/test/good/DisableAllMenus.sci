//  Scicos
//
//  Copyright (C) INRIA - METALAU Project <scicos@inria.fr>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// See the file ../license.txt
//

function DisableAllMenus()
    //** Serge Steer Sept 2009,
    //   use the uimenu properties instead of unsetmenu
    //   No more called when window deletion has been requested (see scicos)


    curf=gcf(); //preserve current figure

    //loop on all opened scicos windows
    for win = intersect(winsid(), [inactive_windows(2);curwin]')
        C=get(scf(win),"children")
        for k=1:size(C,"*")
            if C(k).type=="uimenu" then
                C(k).Enable = "off";
            end
        end
    end
    scf(curf);//restore current figure
endfunction