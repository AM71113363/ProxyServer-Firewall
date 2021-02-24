<p align="center">
<img align="center" width="358" height="215" src="https://raw.githubusercontent.com/AM71113363/ProxyServer-Firewall/master/info.png">
</p>

# ProxyServer-Firewall
-----

## How it Works?<br>
1. Phone and PC have to be connected to the same Wifi network.(PC Wifi/LAN).<br>
2. Run ProxyServer-Firewall.exe (32Bit Windows).<br>
3. The App will show the IP and Port to use as Proxy-Settings on your Phone.<br>
4. CLick [ START ].<br>

## Settings<br>
* Double CLick on Hostname(Client LIST) to add it as a FILTER.<br>
* Double CLick the FILTER to delete it.<br>
* Click twice to rename it.<br>
* if FILTER is "abcd" the Proxy will block all hostames that contain the String "abcd".<br>
* Enable "IgnoreClients" to test the App. Will show the requested hostnames, but NO Connection will be forwarded.<br>
* Enable "IgnoreBlocked" to hide CLients calling blocked Hostnames.<br>
* Set Filename for FILTERS, DEFAULT="Filters.dat".<br>
* CLick "SAVE" to save the FILTERS in the same DIR where the App is.<br>
* Drag-Drop filter_file to use it.<br>
* Check/Uncheck the FILTER to Enable/Disable it.<br>
* Click "Clear" to clear the Client LIST.(Running Threads should be 0 when clicking this).<br>

## Build.
_for beginners_ <br>
1.Download [DevCpp_v4.9.9.*](http://www.bloodshed.net/) and install it.<br>
2.Run build.bat (it works only with Compiler:  Dev-C++ 4.9.9.*).<br>
_others_ <br>
You already know how to rebuilt it. ^_^<br>
<br>

# NOTE
128 is the MAX_FILTER in one file, you can create multiple files, with diff names.
