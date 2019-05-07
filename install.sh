s="[Desktop Entry]\nName=海天鹰录屏\nComment=\nExec=`pwd`/HTYScreenRecorder\nIcon=`pwd`/icon.svg\nPath=`pwd`\nTerminal=false\nType=Application\nCategories=Graphics;"
echo -e $s > HTYScreenRecorder.desktop
cp `pwd`/HTYScreenRecorder.desktop ~/.local/share/applications/HTYScreenRecorder.desktop