#g++ -g -I/usr/local/include/mapnik -I/opt/local/include -I/opt/local/include/freetype2 -I../../agg/include -L/usr/local/lib -L/opt/local/lib -lmapnik  -lboost_thread-mt -licuuc cali.cpp -o cali
g++ -g -m64 -DBIGINT -g -I/usr/local/include/mapnik -I/opt/local/include -I/opt/local/include/freetype2 -I../../agg/include -L/usr/local/lib -L/opt/local/lib -L/cygdrive/f/dev/mapnik/cygwin64/mapnik.v3.x.xx.D.git/src  cali.cpp -o cali.exe -Wl,-dll-search-prefix=cyg -Wl,--start-group -lmapnik -lboost_thread -licuuc -Wl,--end-group
echo "this does not run for me (under cygwin64, win10) with the links "
echo "that the mapnik build creates.  I had to remove the (/as attrib)"
echo "cygmapnik.dll link file (contained unicode string ref'ing the"
echo "cygmapnik.dll.3.1.0 file), and copy the cygmapnik.dll.3.1.0 to"
echo "cygmapnik.dll.  The application would then start (though still trying"
echo "to locate/recreate the missing info.png file.)" 
