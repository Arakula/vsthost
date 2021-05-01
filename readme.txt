VSTHost is a program to load VST PlugIns.

Copyright (C) 2006  Hermann Seib

===============================================================================
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
===============================================================================

The source code can't be compiled without additional resources; it relies
on two things:

1) the Steinberg VST SDK
2) the Steinberg ASIO SDK
These can be downloaded from Steinberg.
Last time I checked, it could be found on http://www.steinberg.de under
"Company / 3rd Party Developers".

VSTHost can use the VST SDK in any version up to VST SDK 2.4 rev2.
The current version, compiled in the Visual Studio 6 workspace vsthost.dsw using
the project file vsthost.dsp, uses VST SDK 2.4 rev2.
Since not everybody wants to switch to this one, there are additional 
workspaces for use with the VST SDK 2.3 and 2.4 rev1.

Each of these workspaces wants the SDK Files in a specific directory 
structure relative to VSTHost:

  Project        VST SDK location  ASIO SDK location
  --------------------------------------------------
  vsthost.dsp    ..\vstsdk2.4.2    ..\asiosdk2
  vsthost24.dsp  ..\vstsdk2.4      ..\asiosdk2
  vsthost23.dsp  ..\vstsdk2.3      ..\asiosdk2

If you have them in another location, you can simply run a global
search-and-replace over the .dsp file (yeh... even if it says
"** DO NOT EDIT **" :-) to adjust these to your location.
If you happen to use an older or newer version of the SDKs, you're a bit on
your own - the Steinberg guys tend to move the files around, and change the
directory structure habitually.